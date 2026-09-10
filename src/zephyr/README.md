# OSAL Zephyr backend

## Required Kconfig options

Applications linking against `osal` on Zephyr need the following in
their `prj.conf`:

```
CONFIG_HEAP_MEM_POOL_SIZE=<size>
CONFIG_THREAD_STACK_INFO=y
CONFIG_DYNAMIC_THREAD=y
CONFIG_DYNAMIC_THREAD_ALLOC=y
CONFIG_EVENTS=y
CONFIG_THREAD_NAME=y
CONFIG_LOG=y
```

- `HEAP_MEM_POOL_SIZE`: backs `k_malloc`/`k_free`, used throughout `osal.c`.
- `THREAD_STACK_INFO`: required by `DYNAMIC_THREAD`.
- `DYNAMIC_THREAD` + `DYNAMIC_THREAD_ALLOC`: `os_thread_create()` takes a
  runtime stack size, so stacks are heap-allocated via
  `k_thread_stack_alloc()` rather than declared statically.
- `EVENTS`: backs `os_event_*()` (`k_event`).
- `THREAD_NAME`: `os_thread_create()` sets the Zephyr thread name from
  its `name` argument.
- `LOG`: backs `os_log()` (`osal_log.c`). Without it, log calls compile
  down to no-ops.

## Linking

Link against `osal` normally:

```cmake
target_link_libraries(app PRIVATE osal)
```
