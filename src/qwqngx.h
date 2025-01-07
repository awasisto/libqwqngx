#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define QWQNGX_RESULT_OK 0
#define QWQNGX_RESULT_ERROR -1

typedef struct qwqngx qwqngx;

int qwqngx_init(qwqngx** q);
void qwqngx_free(qwqngx* q);
int qwqngx_clear(qwqngx* q);
int qwqngx_reset(qwqngx* q);
int qwqngx_randbits(qwqngx* q, int* dest, int length);
int qwqngx_randbytes(qwqngx* q, char* dest, int length);
int qwqngx_randintegers(qwqngx* q, int* dest, int length, int min, int max);
int qwqngx_randuniform(qwqngx* q, double* dest, int length, double min, double max);
int qwqngx_randnormal(qwqngx* q, double* dest, int length, double mean, double stddev);
int qwqngx_runtime_info(qwqngx* q, float* dest);
int qwqngx_diagnostics(qwqngx* q, char dx_code, char* dx_info);
char* qwqngx_status_string(qwqngx* q);
char* qwqngx_device_id(qwqngx* q);

#ifdef __cplusplus
}
#endif
