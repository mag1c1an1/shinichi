use axum::{
    Router,
    routing::{Route, get},
};

#[tokio::main]
async fn main() {
    let app = Router::new().route("/", get(|| async { "welcome to shinichi db" }));
    let listener = tokio::net::TcpListener::bind("0.0.0.0:3000").await.unwrap();
    axum::serve(listener, app).await.unwrap();
}
