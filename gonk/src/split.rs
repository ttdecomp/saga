use std::collections::{HashMap, HashSet};

use anyhow::Context;
use iced_x86::{Decoder, DecoderOptions, Instruction, Mnemonic, OpKind};
use object::{Object, ObjectSection as _, ObjectSymbol as _, RelocationFlags, elf::R_386_PC32, write::Relocation};

#[derive(Debug, serde::Deserialize)]
struct CompileCommand {
    directory: String,
    file: String,
    output: String,
}

/// construct a new object file containing only the specified symbols from the original object file
fn make_object<'a>(
    lib: &Lib<'_, '_>,
    symbols: &[impl AsRef<str> + std::fmt::Debug],
) -> anyhow::Result<object::write::Object<'a>> {
    log::info!("making object with symbols: {:?}", symbols);

    let mut obj = object::write::Object::new(
        object::BinaryFormat::Elf,
        object::Architecture::I386,
        object::Endianness::Little,
    );

    let mut sections = HashMap::new();

    for symbol in symbols {
        let Some(symbol) = lib.symbols_by_name.get(symbol.as_ref()) else {
            log::warn!("Failed to find symbol '{symbol:?}' in original object, skipping");
            continue;
        };

        let name = symbol.name().context("Failed to get symbol name")?;

        let Some(section_idx) = symbol.section().index() else {
            log::warn!("failed to find section for symbol {}, skipping", name);
            continue;
        };

        let section = lib
            .file
            .section_by_index(section_idx)
            .context("Failed to get symbol section")?;

        let write_id = *sections
            .entry(section.name().context("Failed to get section name")?)
            .or_insert_with(|| {
                obj.add_section(
                    Vec::new(),
                    section.name().unwrap().as_bytes().to_vec(),
                    section.kind(),
                )
            });

        // symbols to make relocs against
        // map of original address to dummy symbol id
        let mut dummy_symbols: HashMap<u64, object::write::SymbolId> = HashMap::new();

        let offset = if section.kind() == object::SectionKind::UninitializedData {
            obj.append_section_bss(write_id, symbol.size(), 1)
        } else {
            let bytes = section.data().context("Failed to get section data")?;
            let bytes = &bytes[(symbol.address() - section.address()) as usize
                ..(symbol.address() - section.address() + symbol.size()) as usize];

            let current_symbol_offset = obj.append_section_data(write_id, bytes, 8);

            let mut decoder = Decoder::with_ip(32, bytes, symbol.address(), DecoderOptions::NONE);
            let mut instruction = Instruction::default();
            while decoder.can_decode() {
                decoder.decode_out(&mut instruction);

                match instruction.mnemonic() {
                    Mnemonic::Call => match instruction.op0_kind() {
                        OpKind::NearBranch32 => {
                            let target = instruction.near_branch_target();
                            if let Some(target_symbol) = lib.symbols_by_address.get(&target) {
                                // a dummy symbol in the written object file to make a reloc against
                                let target_reloc_symbol = if let Some(id) = dummy_symbols.get(&target) {
                                    *id
                                } else {
                                    let id = obj.add_symbol(object::write::Symbol {
                                        name: target_symbol.name().context("Failed to get target symbol name")?.as_bytes().to_vec(),
                                        value: 0,
                                        size: 0,
                                        kind: object::SymbolKind::Text,
                                        scope: object::SymbolScope::Dynamic,
                                        weak: false,
                                        section: object::write::SymbolSection::Undefined,
                                        flags: object::SymbolFlags::None,
                                    });

                                    dummy_symbols.insert(target, id);
                                    id
                                };

                                obj.add_relocation(write_id, Relocation {
                                    offset: current_symbol_offset + (decoder.position() - instruction.len()) as u64 + 1,
                                    symbol: target_reloc_symbol,
                                    addend: -4,
                                    flags: RelocationFlags::Elf { r_type: R_386_PC32 },
                                }).context("Failed to add relocation")?;
                            } else {
                                log::warn!("failed to find target symbol for near relative call to 0x{target:x} in {name} (0x{:x}) in original object, skipping", symbol.address());
                            }
                        },
                        _ => (),
                    }
                    _ => (),
                }
            }

            current_symbol_offset
        };

        // add symbol to output
        obj.add_symbol(object::write::Symbol {
            name: name.as_bytes().to_vec(),
            value: offset,
            size: symbol.size(),
            kind: symbol.kind(),
            scope: symbol.scope(),
            weak: symbol.is_weak(),
            section: object::write::SymbolSection::Section(write_id),
            flags: object::SymbolFlags::None,
        });
    }

    Ok(obj)
}

#[derive(serde::Serialize)]
struct ObjDiff {
    build_base: bool,
    build_targets: bool,
    units: Vec<ObjDiffUnit>,
    custom_make: String,
    custom_args: Vec<String>,
}

#[derive(serde::Serialize)]
struct ObjDiffUnit {
    name: String,
    target_path: String,
    base_path: String,
    scratch: Option<ObjDiffScratch>,
}

#[derive(serde::Serialize)]
struct ObjDiffScratch {
    platform: String,
    compiler: String,
    c_flags: String,
}

fn symbol_filter<'a>(symbol: &impl object::ObjectSymbol<'a>) -> bool {
    let Ok(name) = symbol.name() else {
        return false;
    };

    if name.is_empty()
        || name.starts_with("__x86.get_pc_thunk")
        || ["__discard", "_GLOBAL_OFFSET_TABLE_"].contains(&name)
        || symbol.size() == 0
    {
        return false;
    }

    match symbol.kind() {
        object::SymbolKind::Text | object::SymbolKind::Data => {}
        k => {
            log::warn!("skipping symbol '{name}' of unsupported type {k:?}");
            return false;
        }
    };

    true
}

type ElfSymbol<'data, 'file, E> =
    object::read::elf::ElfSymbol<'data, 'file, object::elf::FileHeader32<E>>;

struct Lib<'data, 'file> {
    file: &'file object::read::elf::ElfFile32<'file>,
    symbols_by_name: HashMap<&'data str, ElfSymbol<'data, 'file, object::Endianness>>,
    symbols_by_address: HashMap<u64, ElfSymbol<'data, 'file, object::Endianness>>,
}

pub fn split() -> anyhow::Result<()> {
    let contents = std::fs::read_to_string("build/compile_commands.json")
        .context("Failed to read compile_commands.json")?;
    let compile_commands: Vec<CompileCommand> =
        serde_json::from_str(&contents).context("Failed to parse compile_commands.json")?;

    let contents = std::fs::read("res/libTTapp.so").context("Failed to read libTTapp.so")?;
    let orig_lib_file = object::read::elf::ElfFile32::parse(&*contents)
        .context("Failed to parse original libTTapp.so")?;

    let orig_symbols: HashMap<_, _> = orig_lib_file
        .symbols()
        .map(|sym| (sym.name().unwrap(), sym))
        .collect();

    let original_lib = Lib {
        file: &orig_lib_file,
        symbols_by_name: orig_symbols.clone(),
        symbols_by_address: orig_symbols.into_iter().map(|(name, symbol)| (symbol.address(), symbol)).collect(),
    };

    std::fs::create_dir_all("build/split").context("Failed to create build/split directory")?;

    let mut objdiff_units = vec![];
    let mut used_symbols = HashSet::<String>::new();

    for command in compile_commands.iter() {
        log::info!("processing file '{}'", command.file);

        let binary = std::path::Path::new(&command.directory).join(&command.output);

        let contents = std::fs::read(&binary)
            .with_context(|| format!("Failed to read object file: {}", command.output))?;
        let elf = object::read::elf::ElfFile32::<'_, object::LittleEndian, _>::parse(&*contents)
            .context("Failed to parse ELF file")?;

        let symbols = elf
            .symbols()
            .filter(symbol_filter)
            .filter_map(|s| s.name().ok())
            .map(|s| s.to_string())
            .collect::<Vec<String>>();

        let new_obj = make_object(&original_lib, &symbols)?;

        used_symbols.extend(symbols);

        let output_path = std::path::Path::new("build/split")
            .join(std::path::Path::new(&command.output).file_name().unwrap());

        let bytes = new_obj.write()?;
        std::fs::write(output_path, bytes).context("Failed to write split object file")?;

        let name = std::path::Path::new(&command.file)
            .components()
            .collect::<Vec<_>>();
        let name =
            name[name.len() - 3..]
                .iter()
                .fold(std::path::PathBuf::new(), |mut acc, comp| {
                    acc.push(comp);
                    acc
                });

        objdiff_units.push(ObjDiffUnit {
            name: name.display().to_string(),
            target_path: format!(
                "build/split/{}",
                std::path::Path::new(&command.output)
                    .file_name()
                    .unwrap()
                    .to_string_lossy()
            ),
            base_path: binary.display().to_string(),
            scratch: Some(ObjDiffScratch {
                platform: String::from("android_x86"),
                compiler: String::from("ndk-r8e-gcc-4.7"),
                c_flags: String::new(),
            }),
        });
    }

    // add a unit for the remaining symbols
    let remaining_symbols = orig_lib_file
        .symbols()
        .filter(symbol_filter)
        .filter_map(|s| s.name().ok())
        .filter(|&name| !used_symbols.contains(name))
        .collect::<Vec<_>>();

    log::info!("remaining symbols: {remaining_symbols:?}");

    let remaining = make_object(&original_lib, &remaining_symbols)?;
    let bytes = remaining.write()?;

    std::fs::write("build/split/remaining.c.o", bytes).context("Failed to write remaining.o")?;

    objdiff_units.push(ObjDiffUnit {
        name: "remaining".to_string(),
        target_path: "build/split/remaining.c.o".to_string(),
        base_path: String::from("remaining.c.o"),
        scratch: None,
    });

    let json = std::io::BufWriter::new(
        std::fs::File::create("objdiff.json").context("Failed to open objdiff.json")?,
    );

    serde_json::to_writer(
        json,
        &ObjDiff {
            build_base: true,
            build_targets: false,
            units: objdiff_units,
            custom_make: String::from("sh"),
            custom_args: vec![
                "-c".to_owned(),
                "cmake -B build && cmake --build build -- ".to_owned(),
            ],
        },
    )
    .context("Failed to write objdiff.json")?;

    println!("Wrote objdiff.json");

    Ok(())
}
