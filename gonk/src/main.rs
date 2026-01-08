use clap::{Parser, Subcommand};

mod split;

#[derive(Parser)]
#[command(version, about, long_about = None)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    Split,
}

fn main() {
    simplelog::TermLogger::init(
        simplelog::LevelFilter::Info,
        simplelog::Config::default(),
        simplelog::TerminalMode::Mixed,
        simplelog::ColorChoice::Auto,
    )
    .expect("Failed to initialize logger");

    let cli = Cli::parse();

    match cli.command {
        Commands::Split => match split::split() {
            Ok(_) => println!("\nSplit successful"),
            Err(e) => println!("\nSplit unsuccessful: {e}"),
        },
    }
}
