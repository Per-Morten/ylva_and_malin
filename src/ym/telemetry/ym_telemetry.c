#include <ym_telemetry.h>
#include <pthread.h>
#include <stdbool.h>
#include <inttypes.h>

static ym_mem_region* ym_telemetry_mem_reg;

static atomic_bool ym_telemetry_continue = ATOMIC_VAR_INIT(false);
static pthread_t ym_telemetry_thread;

static
void
ym_telemetry_log_memory()
{
    for (size_t i = ym_mem_reg_region_heads + 1; i < ym_mem_reg_count; ++i)
    {
        YM_DEBUG("Atomic_flag: %zu", sizeof (atomic_flag));
        YM_DEBUG("Thread id: %d", pthread_self());
        ym_mem_region* region = ym_mem_get_region(i);
        YM_DEBUG("Region: %s", ym_mem_reg_id_str(region->id));
        YM_DEBUG("Size: %" PRIu16 "", region->size);
        YM_DEBUG("Mem: %p", region->mem);
        YM_DEBUG("Used: %" PRIu16 "", region->used);
    }
}

static
void*
ym_telemetry_run(YM_UNUSED void* args)
{
    while (atomic_load_explicit(&ym_telemetry_continue,
                                memory_order_relaxed))
    {
        ym_telemetry_log_memory();
        sleep(1);
    }

    return NULL;
}

ym_errc
ym_telemetry_init(ym_mem_region* region)
{
    ym_telemetry_mem_reg = region;

    // Doing a release just to ensure that this instruction
    // cant be reordered after the create.
    // However, I don't think it can.
    atomic_store_explicit(&ym_telemetry_continue, true,
                          memory_order_release);

    pthread_create(&ym_telemetry_thread, NULL,
                   &ym_telemetry_run, NULL);

    return ym_errc_success;
}

ym_errc
ym_telemetry_shutdown()
{
    atomic_store_explicit(&ym_telemetry_continue, false,
                          memory_order_relaxed);

    int errc = pthread_join(ym_telemetry_thread, NULL);
    if (errc)
        YM_WARN("Failed to join pthread");

    return ym_errc_success;
}
