# Alloc与Allocator

## Alloc

`Alloc`是一个内部的更加贴近底层的内存配置器，实现以字节为单位的内存管理，负责小块内存的分配与管理。

暂时选择简单封装 new 和 delete

## Allocator

`Allocator`为用于给其他组件使用的内存分配器，提供标准化的接口，内部使用`Alloc`进行内存的分配。