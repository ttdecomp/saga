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
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("warn")).init();

    let cli = Cli::parse();

    match cli.command {
        Commands::Split => match split::split() {
            Ok(_) => println!("\nSplit successful"),
            Err(e) => println!("\nSplit unsuccessful: {e:?}"),
        },
    }
}
