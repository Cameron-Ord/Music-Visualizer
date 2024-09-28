#include "../include/fft.hpp"

#ifndef M_PI
#define M_PI 3.14159265359
#endif

FourierTransform::FourierTransform() {
    data.max_ampl = 1.0;
    data.output_len = 0;
    memset(data.hamming_values, 0, sizeof(float) * BUFF_SIZE);
    memset(bufs.fft_in, 0, DOUBLE_BUFF * sizeof(float));
    memset(bufs.in_cpy, 0, DOUBLE_BUFF * sizeof(float));
    memset(bufs.pre_raw, 0, BUFF_SIZE * sizeof(float));
    memset(bufs.processed, 0, HALF_BUFF * sizeof(float));
    memset(bufs.smoothed, 0, HALF_BUFF * sizeof(float));
    memset(bufs.smear, 0, HALF_BUFF * sizeof(float));

    for (size_t i = 0; i < BUFF_SIZE; i++) {
        bufs.out_raw[i] = std::complex<float>(0.0f, 0.0f);
    }

    for (size_t i = 0; i < BUFF_SIZE; i++) {
        bufs.post_raw[i] = std::complex<float>(0.0f, 0.0f);
    }

    calculate_window();
}

FData *FourierTransform::get_data() {
    return &data;
}
FBuffers *FourierTransform::get_bufs() {
    return &bufs;
}

void FourierTransform::fft_func(float *in, size_t stride,
                                std::complex<float> *out, size_t n) {
    if (n == 1) {
        out[0] = in[0];
        return;
    }

    size_t half_n = n / 2;

    fft_func(in, stride * 2, out, half_n);
    fft_func(in + stride, stride * 2, out + half_n, half_n);
    // v = o*x
    // out = e - o*x e + o*x e e| e + o*x o - o*x o o
    for (size_t k = 0; k < n / 2; ++k) {
        float t = (float) k / n;
        std::complex<float> v =
            std::exp(std::complex<float>(0, -2.0f * M_PI * t)) *
            out[k + half_n];
        std::complex<float> e = out[k];
        out[k] = e + v;
        out[k + half_n] = e - v;
    }

} /*fft_func*/

void FourierTransform::fft_push(uint32_t pos, float *audio_data_buffer,
                                int bytes) {
    memcpy(bufs.fft_in, audio_data_buffer + pos, bytes);
}

void FourierTransform::generate_visual() {
    memcpy(bufs.in_cpy, bufs.fft_in, sizeof(float) * DOUBLE_BUFF);
    hamming_window();
    fft_func(bufs.pre_raw, 1, bufs.out_raw, BUFF_SIZE);
    squash_to_log(HALF_BUFF);
    apply_smoothing();
    apply_smear();
} /*generate_visual*/

void FourierTransform::calculate_window() {
    for (int i = 0; i < BUFF_SIZE; ++i) {
        float Nf = (float) BUFF_SIZE;
        float t = (float) i / (Nf - 1);
        data.hamming_values[i] = 0.54 - 0.46 * cosf(2 * M_PI * t);
    }
}

void FourierTransform::hamming_window() {
    /*Iterate for the size of a single channel*/
    for (int i = 0; i < BUFF_SIZE; ++i) {
        int left = i * 2;
        int right = i * 2 + 1;

        bufs.pre_raw[i] = MAX(bufs.in_cpy[left], bufs.in_cpy[right]);
        bufs.pre_raw[i] *= data.hamming_values[i];
    }
}

void FourierTransform::squash_to_log(size_t size) {
    // f32 delta_f = (f32)SR / size;
    //  f32 bin_low  = (f32)(1000.0f / delta_f);
    //  f32 bin_high = (f32)(5000.0f / delta_f);

    float step = 1.06f;
    float lowf = 1.0f;
    size_t m = 0;
    data.max_ampl = 1.0f;

    for (float f = lowf; (size_t) f < size; f = ceilf(f * step)) {
        float fs = ceilf(f * step);
        float a = 0.0f;

        for (size_t q = (size_t) f; q < size && q < (size_t) fs; ++q) {
            float b = amp(bufs.out_raw[q]);
            if (b > a) {
                a = b;
            }
        }

        if (data.max_ampl < a) {
            data.max_ampl = a;
        }

        bufs.processed[m++] = a;
    }

    data.output_len = m;
}

float FourierTransform::amp(std::complex<float> z) {
    float a = fabsf(z.real());
    float b = fabsf(z.imag());
    return logf(a * a + b * b);
} /*amp*/

void FourierTransform::apply_smoothing() {
    const int FPS = 60;
    /*Linear smoothing*/
    for (size_t i = 0; i < data.output_len; ++i) {
        bufs.processed[i] /= data.max_ampl;
        bufs.smoothed[i] +=
            (bufs.processed[i] - bufs.smoothed[i]) * 8 * (1.0 / FPS);
    }
}

void FourierTransform::apply_smear() {
    const int FPS = 60;
    for (size_t i = 0; i < data.output_len; ++i) {
        bufs.smear[i] += (bufs.smoothed[i] - bufs.smear[i]) * 5 * (1.0 / FPS);
    }
}