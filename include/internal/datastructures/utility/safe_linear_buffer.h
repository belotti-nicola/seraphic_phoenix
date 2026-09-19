#ifndef SAFE_LINEAR_BUFFER_H
#define SAFE_LINEAR_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct safe_buffer_t
{
    uint8_t *buffer;
    size_t   buffer_size;

    size_t start;
    size_t out;

} safe_buffer_t;

/**
 * Collega la struttura safe_buffer_t a un blocco di memoria esistente.
 */
bool sb_init(safe_buffer_t *sb, uint8_t *buffer, size_t buffer_size);

/**
 * Copia i dati DAL ring buffer verso un buffer esterno (Lettura / Pop)
 * Gestisce automaticamente il wrap-around se il blocco supera la fine.
 */
size_t sb_copy_out(const safe_buffer_t *sb, size_t offset, uint8_t *dest, size_t len);

/**
 * Copia i dati DA un buffer esterno DENTRO il ring buffer (Scrittura / Push)
 * Gestisce automaticamente il wrap-around se il blocco supera la fine.
 */
size_t sb_copy_in(safe_buffer_t *sb, size_t offset, const uint8_t *src, size_t len);

#endif