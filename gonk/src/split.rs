use std::collections::{HashMap, HashSet};

use anyhow::Context;
use object::{Object, ObjectSection as _, ObjectSymbol as _};

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
        let Some(symbol) = lib.symbols.get(symbol.as_ref()) else {
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

        let offset = if section.kind() == object::SectionKind::UninitializedData {
            obj.append_section_bss(write_id, symbol.size(), 1)
        } else {
            let bytes = section.data().context("Failed to get section data")?;
            let bytes = &bytes[(symbol.address() - section.address()) as usize
                ..(symbol.address() - section.address() + symbol.size()) as usize];

            obj.append_section_data(write_id, bytes, 8)
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
    symbols: HashMap<&'data str, ElfSymbol<'data, 'file, object::Endianness>>,
}

pub fn split() -> anyhow::Result<()> {
    let contents = std::fs::read_to_string("build/compile_commands.json")
        .context("Failed to read compile_commands.json")?;
    let compile_commands: Vec<CompileCommand> =
        serde_json::from_str(&contents).context("Failed to parse compile_commands.json")?;

    let contents = std::fs::read("res/libTTapp.so").context("Failed to read libTTapp.so")?;
    let orig_lib_file = object::read::elf::ElfFile32::parse(&*contents)
        .context("Failed to parse original libTTapp.so")?;

    let orig_symbols = orig_lib_file
        .symbols()
        .map(|sym| (sym.name().unwrap(), sym))
        .collect();

    let original_lib = Lib {
        file: &orig_lib_file,
        symbols: orig_symbols,
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
