use std::{alloc::Layout, fmt::Formatter, panic::RefUnwindSafe, sync::Arc};

pub trait Allocation: RefUnwindSafe + Send + Sync {}

impl<T: RefUnwindSafe + Send + Sync> Allocation for T {}

pub(crate) enum Deallocation {
    Standard(Layout),
    Custom(Arc<dyn Allocation>, usize),
}

impl std::fmt::Debug for Deallocation {
    fn fmt(&self, f: &mut Formatter) -> std::fmt::Result {
        match self {
            Deallocation::Standard(layout) => {
                write!(f, "Deallocation::Standard {layout:?}")
            }
            Deallocation::Custom(_, size) => {
                write!(f, "Deallocation::Custom {{ capacity: {size} }}")
            }
        }
    }
}

// Intel x86_64:
// L2D streamer from L1:
// Loads data or instructions from memory to the second-level cache. To use the streamer,
// organize the data or instructions in blocks of 128 bytes, aligned on 128 bytes.
// - https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-optimization-manual.pdf
/// Cache and allocation multiple alignment size
#[cfg(target_arch = "x86_64")]
pub const ALIGNMENT: usize = 1 << 7;

// This size is same across all hardware for this architecture.
/// Cache and allocation multiple alignment size
#[cfg(target_arch = "aarch64")]
pub const ALIGNMENT: usize = 1 << 7;

#[cfg(test)]
mod tests {
    use super::Deallocation;
    #[test]
    fn test_size_of_deallocation() {
        assert_eq!(
            std::mem::size_of::<Deallocation>(),
            3 * std::mem::size_of::<usize>()
        );
    }
}
