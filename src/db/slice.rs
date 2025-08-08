// SPDX-FileCopyrightText: shinichi Contributors
//
// SPDX-License-Identifier: Apache-2.0

use bytes::Bytes;

/// for now just a wrapper on `Bytes`
/// Slice own the data
/// MAX_LEN is 65535
/// more than it will be trancate
#[derive(PartialEq, Eq)]
pub struct Slice(pub Bytes);

impl Slice {
    pub fn len(&self) -> usize {
        self.0.len()
    }
}

pub trait ToSlice {
    fn to_slice(&self) -> Slice;
}

impl<T: AsRef<[u8]>> ToSlice for T {
    fn to_slice(&self) -> Slice {
        Slice(Bytes::copy_from_slice(self.as_ref()))
    }
}
