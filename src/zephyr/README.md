# OSAL Zephyr backend

## Required Kconfig options

`osal`'s hard requirements (`EVENTS`, `DYNAMIC_THREAD`,
`DYNAMIC_THREAD_ALLOC`, `THREAD_STACK_INFO`, `THREAD_NAME`) are selected
automatically by the module's own `Kconfig` (an invisible, `default y`
`config OSAL` that `select`s them) — nothing to add for these in your
app's `prj.conf`.

Two things are still the application's responsibility, because they
aren't fixed module requirements but depend on how the app actually
uses `osal`:

```
CONFIG_HEAP_MEM_POOL_SIZE=<size>
CONFIG_MAIN_STACK_SIZE=<size>
```

- `HEAP_MEM_POOL_SIZE`: backs `k_malloc`/`k_free`, used throughout
  `osal.c`. Must be large enough for every `os_thread_create()` stack
  (heap-allocated via `k_thread_stack_alloc()`) plus whatever else the
  app allocates — scales with how many threads/mailboxes/etc. the app
  creates through `osal`, so the module can't pick a value for you.
- `MAIN_STACK_SIZE`: if your app does nontrivial work (e.g. calling
  into `osal`/`pnet` init) directly on the main thread rather than in
  a dedicated thread, the default is often too small and silently
  corrupts memory on overflow rather than failing cleanly.

Additionally, `CONFIG_LOG=y` enables `os_log()` (`osal_log.c`) output;
without it, log calls just compile down to no-ops, so it's an
application choice rather than a hard requirement.

## Module setup

Before `find_package(Zephyr ...)`:

```cmake
set(CMAKE_SKIP_INSTALL_RULES ON)
list(APPEND ZEPHYR_EXTRA_MODULES "<path-to-osal>/src")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
```

`CMAKE_SKIP_INSTALL_RULES` is required: osal's `install(EXPORT ...)`
references `zephyr_interface`/`kernel`, which aren't in any export
set, which CMake rejects.

## Linking

Link against `osal` normally:

```cmake
target_link_libraries(app PRIVATE osal)
```
