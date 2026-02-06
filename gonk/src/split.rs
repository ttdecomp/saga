use std::{
    collections::{HashMap, HashSet},
    path::PathBuf,
};

use anyhow::Context;
use iced_x86::{Decoder, DecoderOptions, Instruction, Mnemonic, OpKind, Register};
use object::{
    Endianness, Object, ObjectSection as _, ObjectSymbol as _, RelocationFlags, SectionKind,
    elf::{FileHeader32, R_386_GOT32, R_386_GOTOFF, R_386_GOTPC, R_386_PC32, R_386_PLT32},
    read,
    write::{Relocation, SectionId, Symbol},
};

#[derive(Debug, serde::Deserialize)]
struct CompileCommand {
    directory: PathBuf,
    file: PathBuf,
    output: PathBuf,
}

/// construct a new object file containing only the specified symbols from the original object file
fn make_object<'a>(
    lib: &Lib<'_, '_>,
    text_symbols: &[impl AsRef<str> + std::fmt::Debug],
    data_symbols: &[impl AsRef<str> + std::fmt::Debug],
    should_process_relocs: bool,
) -> anyhow::Result<object::write::Object<'a>> {
    let mut obj = object::write::Object::new(
        object::BinaryFormat::Elf,
        object::Architecture::I386,
        object::Endianness::Little,
    );

    let mut sections = HashMap::new();

    let mut orig_sym_idx_to_new_offset =
        build_data_sections(lib, data_symbols, &mut obj, &mut sections)?;

    // Place .text symbols, editing as necessary to replace offsets, while also
    // gathering relocs.
    let mut all_relocs = HashMap::new();

    for symbol in text_symbols {
        let Some(orig_sym) = lib.symbols_by_name.get(symbol.as_ref()) else {
            log::warn!("Failed to find symbol '{symbol:?}' in original object, skipping");
            continue;
        };

        let name = orig_sym.name().context("Failed to get symbol name")?;

        let Some(section_idx) = orig_sym.section().index() else {
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

        let bytes = section.data().context("Failed to get section data")?;

        let symbol_start = (orig_sym.address() - section.address()) as usize;
        let bytes = &bytes[symbol_start..symbol_start + orig_sym.size() as usize];

        // Don't unnecessarily copy bytes if we're not going to modify them.
        let mut editable_bytes = if should_process_relocs {
            bytes.to_vec()
        } else {
            Vec::new()
        };

        let (bytes, relocs) = if should_process_relocs {
            let relocs = generate_text_relocs(
                lib,
                orig_sym,
                name,
                write_id,
                &mut editable_bytes,
                &mut orig_sym_idx_to_new_offset,
            )?;

            (editable_bytes.as_slice(), relocs)
        } else {
            (bytes, Vec::new())
        };

        let current_symbol_offset = obj.append_section_data(write_id, bytes, 8);

        all_relocs.insert(current_symbol_offset, relocs);

        // add symbol to output
        obj.add_symbol(object::write::Symbol {
            name: name.as_bytes().to_vec(),
            value: current_symbol_offset,
            size: orig_sym.size(),
            kind: orig_sym.kind(),
            scope: orig_sym.scope(),
            weak: orig_sym.is_weak(),
            section: object::write::SymbolSection::Section(write_id),
            flags: object::SymbolFlags::None,
        });

        orig_sym_idx_to_new_offset.insert(orig_sym.index(), current_symbol_offset as u32);
    }

    add_relocs(lib, &mut obj, sections, all_relocs)?;

    Ok(obj)
}

fn add_relocs(
    lib: &Lib<'_, '_>,
    obj: &mut object::write::Object<'_>,
    sections: HashMap<&str, SectionId>,
    all_relocs: HashMap<u64, Vec<Reloc>>,
) -> Result<(), anyhow::Error> {
    let mut external_syms = HashMap::new();
    for (sym_offset, relocs) in all_relocs {
        for reloc in relocs {
            let target = match reloc.target {
                RelocTarget::Symbol(sym_name) => {
                    if let Some(sym_id) = obj.symbol_id(sym_name.as_bytes()) {
                        sym_id
                    } else {
                        // The symbol is not defined in the current file.
                        // Generate an extern symbol as the destination of the
                        // reloc.
                        let orig_sym = lib.symbols_by_name.get(sym_name).unwrap();

                        *external_syms.entry(sym_name).or_insert_with(|| {
                            obj.add_symbol(Symbol {
                                name: orig_sym.name().unwrap().as_bytes().to_vec(),
                                value: 0,
                                size: 0,
                                kind: orig_sym.kind(),
                                scope: orig_sym.scope(),
                                weak: orig_sym.is_weak(),
                                section: object::write::SymbolSection::Undefined,
                                flags: object::SymbolFlags::None,
                            })
                        })
                    }
                }

                RelocTarget::Section(sect_name) => {
                    let sect_id = sections.get(sect_name).unwrap();

                    obj.section_symbol(*sect_id)
                }
            };

            obj.add_relocation(
                reloc.section_id,
                Relocation {
                    offset: sym_offset + reloc.offset,
                    symbol: target,
                    addend: reloc.addend,
                    flags: RelocationFlags::Elf {
                        r_type: reloc.r_type,
                    },
                },
            )
            .context("Failed to add relocation")?;
        }
    }

    Ok(())
}

fn build_data_sections<'data>(
    lib: &Lib<'_, 'data>,
    data_symbols: &[impl AsRef<str> + std::fmt::Debug],
    obj: &mut object::write::Object<'_>,
    sections: &mut HashMap<&'data str, SectionId>,
) -> Result<HashMap<read::SymbolIndex, u32>, anyhow::Error> {
    let mut orig_sym_idx_to_new_offset = HashMap::new();

    for symbol in data_symbols {
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

        let new_offset = match section.kind() {
            SectionKind::Data | SectionKind::ReadOnlyData => {
                let bytes = section.data().context("Failed to get section data")?;

                let symbol_start = (symbol.address() - section.address()) as usize;
                let bytes = &bytes[symbol_start..symbol_start + symbol.size() as usize];

                obj.append_section_data(write_id, bytes, 8)
            }

            SectionKind::UninitializedData => obj.append_section_bss(write_id, symbol.size(), 1),

            _ => continue,
        };

        obj.add_symbol(object::write::Symbol {
            name: name.as_bytes().to_vec(),
            value: new_offset,
            size: symbol.size(),
            kind: symbol.kind(),
            scope: symbol.scope(),
            weak: symbol.is_weak(),
            section: object::write::SymbolSection::Section(write_id),
            flags: object::SymbolFlags::None,
        });

        orig_sym_idx_to_new_offset.insert(symbol.index(), new_offset as u32);
    }

    Ok(orig_sym_idx_to_new_offset)
}

#[derive(Debug)]
enum Rewrite<'a> {
    // An entry in the global offset table.
    Got(&'a str),

    /// An offset from the start of the _GLOBAL_OFFSET_TABLE_ directly to the
    /// symbol or data in memory.
    GotOffset(Option<u32>, &'a str),
}

#[derive(Debug)]
struct Reloc<'a> {
    section_id: SectionId,
    offset: u64,
    addend: i64,
    target: RelocTarget<'a>,
    r_type: u32,
}

#[derive(Debug)]
enum RelocTarget<'a> {
    Symbol(&'a str),
    Section(&'a str),
}

fn generate_text_relocs<'data>(
    lib: &Lib<'_, 'data>,
    orig_sym: &read::elf::ElfSymbol<FileHeader32<Endianness>>,
    name: &str,
    section_id: object::write::SectionId,
    bytes: &mut [u8],
    orig_sym_idx_to_new_offset: &mut HashMap<read::SymbolIndex, u32>,
) -> Result<Vec<Reloc<'data>>, anyhow::Error> {
    let mut decoder = Decoder::with_ip(32, bytes, orig_sym.address(), DecoderOptions::NONE);

    let orig_got_section = lib.file.section_by_name(".got").unwrap();
    let orig_got_symbol_addr = lib
        .symbols_by_name
        .get("_GLOBAL_OFFSET_TABLE_")
        .unwrap()
        .address() as u32;

    let mut local_rewrites = Vec::new();
    let mut relocs = Vec::new();

    // The register which holds the address of the _GLOBAL_OFFSET_TABLE_ symbol.
    let mut thunk_reg = None;

    while decoder.can_decode() {
        let mut instruction = Instruction::default();
        decoder.decode_out(&mut instruction);

        match instruction.mnemonic() {
            Mnemonic::Call => {
                if instruction.op0_kind() == OpKind::NearBranch32 {
                    let target = instruction.near_branch_target();
                    if let Some(target_symbol) = lib.symbols_by_address.get(&target) {
                        let target_name = target_symbol.name().unwrap();

                        if let Some(reg) = get_thunk_reg(target_name) {
                            thunk_reg = Some((reg, orig_sym.address() + decoder.position() as u64));
                        }

                        let r_type = if target_symbol.is_local() {
                            R_386_PC32
                        } else {
                            R_386_PLT32
                        };

                        relocs.push(Reloc {
                            section_id,
                            offset: (decoder.position() - instruction.len()) as u64 + 1,
                            addend: -4,
                            target: RelocTarget::Symbol(target_name),
                            r_type,
                        });
                    } else {
                        log::warn!(
                            "failed to find target symbol for near relative call to 0x{target:x} in {name} (0x{:x}) in original object, skipping",
                            orig_sym.address()
                        );
                    }
                }

                continue;
            }

            Mnemonic::Add | Mnemonic::Sub => {
                let Some((thunk_reg, value)) = thunk_reg else {
                    continue;
                };

                let op0_is_thunk_reg = instruction.op0_kind() == OpKind::Register
                    && instruction.op0_register() == thunk_reg;
                let op1_is_got_offset = instruction.op1_kind() == OpKind::Immediate32
                    && (value as u32).wrapping_add(instruction.immediate32())
                        == orig_got_symbol_addr;

                if op0_is_thunk_reg && op1_is_got_offset {
                    // Add a reloc for the _GLOBAL_OFFSET_TABLE_ symbol.
                    let immediate_off = decoder
                        .get_constant_offsets(&instruction)
                        .immediate_offset();

                    relocs.push(Reloc {
                        section_id,
                        offset: (decoder.position() - instruction.len() + immediate_off) as u64,
                        addend: 2,
                        target: RelocTarget::Symbol("_GLOBAL_OFFSET_TABLE_"),
                        r_type: R_386_GOTPC,
                    });

                    local_rewrites
                        .push((decoder.position() - instruction.len() + immediate_off, None));

                    continue;
                }
            }

            _ => (),
        }

        for op_idx in 0..instruction.op_count() {
            // Only try to generate GOT relocs if there's a memory operand
            // offset from the current thunk register, if any.
            let Some((thunk_reg, _)) = thunk_reg else {
                continue;
            };

            if instruction.op_kind(op_idx) != OpKind::Memory
                || instruction.memory_base() != thunk_reg
            {
                continue;
            }

            let orig_addr =
                orig_got_symbol_addr.wrapping_add(instruction.memory_displacement32()) as u64;

            let rewrite = generate_rewrite(
                lib,
                orig_sym_idx_to_new_offset,
                &orig_got_section,
                orig_addr,
            );

            if let Some(rewrite) = rewrite {
                let (r_type, target, new_addr) = match &rewrite {
                    Rewrite::Got(symbol_name) => {
                        (R_386_GOT32, RelocTarget::Symbol(symbol_name), None)
                    }

                    Rewrite::GotOffset(new_addr, symbol_name) => {
                        if let Some(new_addr) = *new_addr {
                            (
                                R_386_GOTOFF,
                                RelocTarget::Section(symbol_name),
                                Some(new_addr),
                            )
                        } else {
                            (R_386_GOT32, RelocTarget::Symbol(symbol_name), None)
                        }
                    }
                };

                // Rewrite the instruction and add a reloc to point to the new
                // location in the final object.
                let displacement_off = decoder
                    .get_constant_offsets(&instruction)
                    .displacement_offset();

                relocs.push(Reloc {
                    section_id,
                    offset: (decoder.position() - instruction.len() + displacement_off) as u64,
                    addend: 0,
                    target,
                    r_type,
                });

                // We can't rewrite right now, while the decoder is borrowing
                // the data, so record it to do after processing all instrs.
                local_rewrites.push((
                    decoder.position() - instruction.len() + displacement_off,
                    new_addr,
                ));
            }

            break;
        }
    }

    // Rewrite memory addresses to point to the new locations of symbols in the
    // object. We do this by hand instead of using the `iced_x86` encoder as it
    // can create output which differs from the input in more ways than we want.
    for (offset, new_addr) in local_rewrites {
        let new_addr = new_addr.unwrap_or_default();

        for (idx, byte) in new_addr.to_le_bytes().into_iter().enumerate() {
            bytes[offset + idx] = byte;
        }
    }

    Ok(relocs)
}

fn generate_rewrite<'data>(
    lib: &Lib<'_, 'data>,
    orig_sym_idx_to_new_offset: &HashMap<read::SymbolIndex, u32>,
    orig_got_section: &read::elf::ElfSection<'_, '_, FileHeader32<Endianness>>,
    orig_addr: u64,
) -> Option<Rewrite<'data>> {
    if let Some(orig_sym) = lib.symbols_by_address.get(&orig_addr) {
        // The address points directly to a symbol.
        let orig_section = lib
            .file
            .section_by_index(orig_sym.section_index().unwrap())
            .unwrap();

        let new_addr = orig_sym_idx_to_new_offset.get(&orig_sym.index()).copied();

        let symbol_name = if new_addr.is_some() {
            orig_section.name()
        } else {
            orig_sym.name()
        }
        .unwrap();

        Some(Rewrite::GotOffset(new_addr, symbol_name))
    } else if (orig_got_section.address()..orig_got_section.address() + orig_got_section.size())
        .contains(&orig_addr)
    {
        // The address falls within the .got section, suggesting it's a pointer
        // to a symbol defined elsewhere. Calculate the address of the original
        // symbol.
        let byte_idx = orig_addr as usize - orig_got_section.address() as usize;
        let bytes = orig_got_section.data().unwrap();

        let sym_addr = u64::from_le_bytes([
            bytes[byte_idx],
            bytes[byte_idx + 1],
            bytes[byte_idx + 2],
            bytes[byte_idx + 3],
            0,
            0,
            0,
            0,
        ]);

        let orig_sym = lib.symbols_by_address.get(&sym_addr).unwrap();
        let orig_name = orig_sym.name().unwrap();

        Some(Rewrite::Got(orig_name))
    } else {
        None
    }
}

/// Returns the thunk register, if any, set by the current call.
fn get_thunk_reg(target_name: &str) -> Option<Register> {
    let reg = target_name.strip_prefix("__x86.get_pc_thunk.")?;

    let reg = match reg {
        "bx" => Register::EBX,
        "cx" => Register::ECX,
        "dx" => Register::EDX,

        _ => panic!("unrecognized register {reg}"),
    };

    Some(reg)
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
    target_path: PathBuf,
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
        || ["__discard", "_GLOBAL_OFFSET_TABLE_", "_DYNAMIC"].contains(&name)
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
        symbols_by_address: orig_symbols
            .into_values()
            .map(|symbol| (symbol.address(), symbol))
            .collect(),
    };

    std::fs::create_dir_all("build/split").context("Failed to create build/split directory")?;

    let mut objdiff_units = vec![];
    let mut used_symbols: HashSet<_> = HashSet::new();

    for command in compile_commands.iter() {
        log::info!("processing file '{}'", command.file.display());

        let binary = std::path::Path::new(&command.directory).join(&command.output);

        let contents = std::fs::read(&binary)
            .with_context(|| format!("Failed to read object file: {}", command.output.display()))?;
        let elf = read::elf::ElfFile32::<'_, object::LittleEndian, _>::parse(&*contents)
            .context("Failed to parse ELF file")?;

        let (text_symbols, data_symbols): (Vec<_>, Vec<_>) =
            elf.symbols().filter(symbol_filter).partition(|sym| {
                elf.section_by_index(sym.section_index().unwrap())
                    .unwrap()
                    .kind()
                    == SectionKind::Text
            });

        let text_symbols: Vec<_> = text_symbols
            .into_iter()
            .map(|sym| sym.name().unwrap().to_string())
            .collect();

        let data_symbols: Vec<_> = data_symbols
            .into_iter()
            .map(|sym| sym.name().unwrap().to_string())
            .collect();

        let new_obj = make_object(&original_lib, &text_symbols, &data_symbols, true)?;

        used_symbols.extend(text_symbols);
        used_symbols.extend(data_symbols);

        let output_relative_path = command.output.strip_prefix("CMakeFiles/saga.dir").unwrap();
        let output_path = std::path::Path::new("build/split").join(output_relative_path);

        std::fs::create_dir_all(output_path.parent().unwrap())
            .context("Failed to create build/split directory")?;

        let bytes = new_obj.write()?;
        std::fs::write(&output_path, bytes).context("Failed to write split object file")?;

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
            target_path: output_path,
            base_path: binary.display().to_string(),
            scratch: Some(ObjDiffScratch {
                platform: String::from("android_x86"),
                compiler: String::from("ndk-r8e-gcc-4.7"),
                c_flags: String::from("-fno-exceptions -fno-rtti"),
            }),
        });
    }

    // add a unit for the remaining symbols
    let (text_symbols, data_symbols): (Vec<_>, Vec<_>) = orig_lib_file
        .symbols()
        .filter(symbol_filter)
        .filter(|sym| !used_symbols.contains(sym.name().unwrap()))
        .partition(|sym| {
            orig_lib_file
                .section_by_index(sym.section_index().unwrap())
                .unwrap()
                .kind()
                == SectionKind::Text
        });

    let text_symbols: Vec<_> = text_symbols
        .into_iter()
        .map(|sym| sym.name().unwrap().to_string())
        .collect();

    let data_symbols: Vec<_> = data_symbols
        .into_iter()
        .map(|sym| sym.name().unwrap().to_string())
        .collect();

    let remaining = make_object(&original_lib, &text_symbols, &data_symbols, false)?;
    let bytes = remaining.write()?;

    std::fs::write("build/split/remaining.c.o", bytes).context("Failed to write remaining.o")?;

    objdiff_units.push(ObjDiffUnit {
        name: "remaining".to_string(),
        target_path: PathBuf::from("build/split/remaining.c.o"),
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
