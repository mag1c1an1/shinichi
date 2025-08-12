use std::sync::Arc;

use axum::{Router, extract::Path, routing::get};
use parking_lot::Mutex;
use shinichi::DB;
use snafu::{ResultExt, Whatever};

async fn get_val(Path(key): Path<String>, db: Arc<Mutex<DB>>) -> String {
    "value".into()
}

async fn get_val(Path(key): Path<String>, db: Arc<Mutex<DB>>) -> String {
    "value".into()
}

#[tokio::main]
#[snafu::report]
async fn main() -> Result<(), Whatever> {
    let db = Arc::new(Mutex::new(
        shinichi::DB::open("server.shi").whatever_context("failed open ")?,
    ));
    let app = Router::new()
        .route("/", get(|| async { "welcome to shinichi db" }))
        .route(
            "/kv/{key}",
            get({
                let db = Arc::clone(&db);
                move |path| get_val(path, db)
            }),
        );
    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000").await.unwrap();
    axum::serve(listener, app).await.unwrap();
    Ok(())
}
