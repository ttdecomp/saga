use anyhow::Context;
use object::{Object, ObjectSection as _, ObjectSymbol as _};

#[derive(Debug, serde::Deserialize)]
struct CompileCommand {
    directory: String,
    file: String,
    output: String,
}

/// construct a new object file containing only the specified symbols from the original object file
pub fn make_object<'a>(
    lib: &object::read::elf::ElfFile32<'_>,
    symbols: &[&str],
) -> anyhow::Result<object::write::Object<'a>> {
    log::info!("making object with symbols: {:?}", symbols);

    let mut obj = object::write::Object::new(
        object::BinaryFormat::Elf,
        object::Architecture::X86_64,
        object::Endianness::Little,
    );

    let lib_section = lib
        .section_by_name(".text")
        .context("Failed to find .text section")?;

    let new_section = obj.add_section(vec![], b".text".to_vec(), object::SectionKind::Text);

    for symbol in symbols {
        let Some(symbol) = lib.symbol_by_name(symbol) else {
            log::warn!("Failed to find symbol '{symbol}' in original object, skipping");
            continue;
        };

        let data_offset = (symbol.address() - lib_section.address()) as usize;
        let data =
            &lib_section.data().unwrap_or(&[])[data_offset..data_offset + symbol.size() as usize];

        let offset = obj.append_section_data(new_section, data, 8);

        // add symbol to output
        obj.add_symbol(object::write::Symbol {
            name: symbol
                .name()
                .context("Failed to get symbol name")?
                .as_bytes()
                .to_vec(),
            value: offset,
            size: symbol.size(),
            kind: object::SymbolKind::Text,
            scope: object::SymbolScope::Dynamic,
            weak: false,
            section: object::write::SymbolSection::Section(new_section),
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
}

#[derive(serde::Serialize)]
struct ObjDiffUnit {
    name: String,
    target_path: String,
    base_path: String,
}

pub fn split() -> anyhow::Result<()> {
    let contents = std::fs::read_to_string("build/compile_commands.json")
        .context("Failed to read compile_commands.json")?;
    let compile_commands: Vec<CompileCommand> =
        serde_json::from_str(&contents).context("Failed to parse compile_commands.json")?;

    let contents = std::fs::read("res/libTTapp.so").context("Failed to read libTTapp.so")?;
    let original_lib = object::read::elf::ElfFile32::parse(&*contents)
        .context("Failed to parse original libTTapp.so")?;

    let mut objdiff_units = vec![];

    for command in compile_commands.iter() {
        log::info!("processing file '{}'", command.file);

        let binary = std::path::Path::new(&command.directory).join(&command.output);

        let contents = std::fs::read(&binary)
            .with_context(|| format!("Failed to read object file: {}", command.output))?;
        let elf = elf::ElfBytes::<elf::endian::LittleEndian>::minimal_parse(&contents)
            .context("Failed to parse ELF file")?;

        let common = elf
            .find_common_data()
            .context("Failed to find common data")?;

        let strings = common.symtab_strs.unwrap();

        let mut symbols = vec![];

        for sym in common.symtab.unwrap() {
            let name = strings
                .get(sym.st_name as usize)
                .context("Failed to get symbol name")?;

            log::debug!("found symbol '{:?}'", sym);

            if name.is_empty()
                || name.starts_with("__x86.get_pc_thunk")
                || ["__discard", "_GLOBAL_OFFSET_TABLE_"].contains(&name)
                || sym.st_size == 0
                || sym.st_info & 0xf != elf::abi::STT_FUNC
            {
                log::debug!("skipping symbol '{}'", name);
                continue;
            }

            symbols.push(name);
        }

        let new_obj = make_object(&original_lib, &symbols)?;

        std::fs::create_dir_all("build/split").context("Failed to create build/split directory")?;

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

    std::fs::write(
        "objdiff.json",
        serde_json::to_string(&ObjDiff {
            build_base: false,
            build_targets: true,
            units: objdiff_units,
        })?,
    )
    .context("Failed to write objdiff.json")?;

    println!("Wrote objdiff.json");

    Ok(())
}
