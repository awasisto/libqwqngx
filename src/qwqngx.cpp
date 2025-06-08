#include "qwqngx.h"

#include <memory>
#include <qwqng.hpp>

#define ZIGNOR_C 128
#define ZIGNOR_R 3.442619855899
#define ZIGNOR_V 9.91256303526217e-3

struct qwqngx {
    QWQNG* qwqng;
    int device_max_bytes_to_request;
    double ad_zig_x[ZIGNOR_C + 1];
    double ad_zig_r[ZIGNOR_C];
};

static double to_double(const uint64_t* uint64_buffer)
{
    return (double)(*uint64_buffer >> 11) * 0x1.0p-53;
}

int qwqngx_init(qwqngx** q)
{
    *q = (qwqngx*)malloc(sizeof(qwqngx));
    if (!*q) {
        return QWQNGX_RESULT_ERROR;
    }

    QWQNG* qwqng = new QWQNG();

    (*q)->qwqng = qwqng;

    std::string device_id = (*q)->qwqng->DeviceID();
    if (device_id.empty()) {
        delete (*q)->qwqng;
        (*q)->qwqng = nullptr;
        free(*q);
        *q = nullptr;
        return QWQNGX_RESULT_ERROR;
    }
    (*q)->device_max_bytes_to_request = device_id.length() >= 4 && device_id[3] >= '7' ? FTDIDEVICE_FAST_MAX_ARRAY_SIZE_ : FTDIDEVICE_MAX_ARRAY_SIZE_;

    double f = exp(-0.5 * ZIGNOR_R * ZIGNOR_R);
    (*q)->ad_zig_x[0] = ZIGNOR_V / f;
    (*q)->ad_zig_x[1] = ZIGNOR_R;
    (*q)->ad_zig_x[ZIGNOR_C] = 0;
    for (int i = 2; i < ZIGNOR_C; ++i) {
        (*q)->ad_zig_x[i] = sqrt(-2 * log(ZIGNOR_V / (*q)->ad_zig_x[i - 1] + f));
        f = exp(-0.5 * (*q)->ad_zig_x[i] * (*q)->ad_zig_x[i]);
    }
    for (int i = 0; i < ZIGNOR_C; ++i) {
        (*q)->ad_zig_r[i] = (*q)->ad_zig_x[i + 1] / (*q)->ad_zig_x[i];
    }

    return QWQNGX_RESULT_OK;
}

void qwqngx_free(qwqngx* q)
{
    if (q) {
        if (q->qwqng) {
            delete q->qwqng;
            q->qwqng = nullptr;
        }
        free(q);
        q = nullptr;
    }
}

int qwqngx_clear(qwqngx* q)
{
    if (q->qwqng->Clear() != S_OK) {
        return QWQNGX_RESULT_ERROR;
    }
    return QWQNGX_RESULT_OK;
}

int qwqngx_reset(qwqngx* q)
{
    if (q->qwqng->Reset() != S_OK) {
        return QWQNGX_RESULT_ERROR;
    }
    return QWQNGX_RESULT_OK;
}

int qwqngx_randbits(qwqngx* q, int* dest, int length)
{
    int buffer_length = (length + 7) >> 3;
    std::unique_ptr<char[]> buffer(new char[buffer_length]);
    if (qwqngx_randbytes(q, buffer.get(), buffer_length) != S_OK) {
        return QWQNGX_RESULT_ERROR;
    }
    for (int i = 0; i < length; ++i) {
        dest[i] = (buffer[i >> 3] >> (i & 7)) & 1;
    }
    return QWQNGX_RESULT_OK;
}

int qwqngx_randbytes(qwqngx* q, char* dest, int length)
{
    int requests_to_make = length / q->device_max_bytes_to_request;
    if (length % q->device_max_bytes_to_request != 0) {
        ++requests_to_make;
    }
    for (int requests_made = 0; requests_made < requests_to_make; ++requests_made) {
        int offset = requests_made * q->device_max_bytes_to_request;
        int bytes_to_request = length - offset;
        if (bytes_to_request > q->device_max_bytes_to_request) {
            bytes_to_request = q->device_max_bytes_to_request;
        }
        if (q->qwqng->RandBytes(dest + offset, bytes_to_request) != S_OK) {
            return QWQNGX_RESULT_ERROR;
        }
    }
    return QWQNGX_RESULT_OK;
}

int qwqngx_randintegers(qwqngx* q, int* dest, int length, int min, int max)
{
    uint32_t s = max - min + 1;
    char byte_buffer[4];
    if (s != 0) {
        for (int i = 0; i < length; ++i) {
            // M.E. O'Neill. "Efficiently Generating a Number in a Range". PCG, A Better Random Number Generator.
            // https://www.pcg-random.org/posts/bounded-rands.html
            if (q->qwqng->RandBytes(byte_buffer, 4) != S_OK) {
                return QWQNGX_RESULT_ERROR;
            }
            uint64_t m = (uint64_t)*((uint32_t*)byte_buffer) * (uint64_t)s;
            uint32_t l = m;
            if (l < s) {
                uint32_t t = -s;
                if (t >= s) {
                    t -= s;
                    if (t >= s) {
                        t %= s;
                    }
                }
                while (l < t) {
                    if (q->qwqng->RandBytes(byte_buffer, 4) != S_OK) {
                        return QWQNGX_RESULT_ERROR;
                    }
                    m = (uint64_t)*((uint32_t*)byte_buffer) * (uint64_t)s;
                    l = (uint32_t)m;
                }
            }
            dest[i] = min + (int32_t)(m >> 32);
        }
    } else {
        for (int i = 0; i < length; ++i) {
            if (q->qwqng->RandBytes(byte_buffer, 4) != S_OK) {
                return QWQNGX_RESULT_ERROR;
            }
            dest[i] = *((int32_t*)byte_buffer);
        }
    }
    return QWQNGX_RESULT_OK;
}

int qwqngx_randuniform(qwqngx* q, double* dest, int length, double min, double max)
{
    int bytes_needed = length << 3;
    std::unique_ptr<char[]> byte_buffer = std::make_unique<char[]>(bytes_needed);
    if (qwqngx_randbytes(q, byte_buffer.get(), bytes_needed) != S_OK) {
        return QWQNGX_RESULT_ERROR;
    }
    double s = max - min;
    auto uint64_buffer = (uint64_t*)byte_buffer.get();
    for (int numbers_generated = 0; numbers_generated < length; ++numbers_generated) {
        dest[numbers_generated] = min + to_double(uint64_buffer + numbers_generated) * s;
    }
    return QWQNGX_RESULT_OK;
}

int qwqngx_randnormal(qwqngx* q, double* dest, int length, double mean, double stddev)
{
    char byte_buffer[8];
    for (int i = 0; i < length; ++i) {
        // J. Doornik, "An Improved Ziggurat Method to Generate Normal Random Samples", University of Oxford, 2005.
        for (;;) {
            if (q->qwqng->RandBytes(byte_buffer, 8) != S_OK) {
                return QWQNGX_RESULT_ERROR;
            }
            double u = 2 * to_double((uint64_t*)byte_buffer) - 1;
            if (q->qwqng->RandBytes(byte_buffer, 4) != S_OK) {
                return QWQNGX_RESULT_ERROR;
            }
            uint32_t blkIdx = *((uint32_t*)byte_buffer) & 0x7F;
            if (fabs(u) < q->ad_zig_r[blkIdx]) {
                dest[i] = mean + u * q->ad_zig_x[blkIdx] * stddev;
                break;
            }
            if (blkIdx == 0) {
                double x, y;
                do {
                    if (q->qwqng->RandBytes(byte_buffer, 8) != S_OK) {
                        return QWQNGX_RESULT_ERROR;
                    }
                    x = log(to_double((uint64_t*)byte_buffer)) / ZIGNOR_R;
                    if (q->qwqng->RandBytes(byte_buffer, 8) != S_OK) {
                        return QWQNGX_RESULT_ERROR;
                    }
                    y = log(to_double((uint64_t*)byte_buffer));
                } while (-2 * y < x * x);
                dest[i] = mean + (u < 0 ? x - ZIGNOR_R : ZIGNOR_R - x) * stddev;
                break;
            }
            double x = u * q->ad_zig_x[blkIdx];
            double f0 = exp(-0.5 * (q->ad_zig_x[blkIdx] * q->ad_zig_x[blkIdx] - x * x));
            double f1 = exp(-0.5 * (q->ad_zig_x[blkIdx + 1] * q->ad_zig_x[blkIdx + 1] - x * x));
            if (int qwqngStatus = q->qwqng->RandBytes(byte_buffer, 8) != S_OK) {
                return qwqngStatus;
            }
            if (f1 + to_double((uint64_t*)byte_buffer) * (f0 - f1) < 1.0) {
                dest[i] = mean + x * stddev;
                break;
            }
        }
    }
    return QWQNGX_RESULT_OK;
}

int qwqngx_runtime_info(qwqngx* q, float* dest)
{
    if (q->qwqng->RuntimeInfo(dest) != S_OK) {
        return QWQNGX_RESULT_ERROR;
    }
    return QWQNGX_RESULT_OK;
}

int qwqngx_diagnostics(qwqngx* q, char dx_code, char* dx_info)
{
    if (q->qwqng->Diagnostics(dx_code, dx_info) != S_OK) {
        return QWQNGX_RESULT_ERROR;
    }
    return QWQNGX_RESULT_OK;
}

char* qwqngx_status_string(qwqngx* q)
{
    return q->qwqng->StatusString();
}

char* qwqngx_device_id(qwqngx* q)
{
    return q->qwqng->DeviceID();
}
