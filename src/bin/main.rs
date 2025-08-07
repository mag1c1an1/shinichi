use std::{
    sync::{Arc, atomic::AtomicI32},
    thread,
};

// SPDX-FileCopyrightText: LakeSoul Contributors
//
// SPDX-License-Identifier: Apache-2.0
fn one() -> bool {
    let y = Arc::new(AtomicI32::new(0));
    let x = Arc::new(AtomicI32::new(0));

    let j1 = {
        let y = Arc::clone(&y);
        let x = Arc::clone(&x);
        thread::spawn(move || {
            let r1 = y.load(std::sync::atomic::Ordering::Relaxed);
            x.store(r1, std::sync::atomic::Ordering::Relaxed);
            r1
        })
    };
    let j2 = {
        let y = Arc::clone(&y);
        let x = Arc::clone(&x);
        thread::spawn(move || {
            let r2 = x.load(std::sync::atomic::Ordering::Relaxed);
            y.store(42, std::sync::atomic::Ordering::Relaxed);
            r2
        })
    };
    let r1 = j1.join().unwrap();
    let r2 = j2.join().unwrap();
    r1 == r2 && r1 == 42
}

fn main() {
    loop {
        if one() {
            print!("it is truth");
            break;
        }
    }
}
