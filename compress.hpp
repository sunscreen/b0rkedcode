#pragma once
#ifndef COMPRESS_H
#define COMPRESS_H

class Compressor {
    public:

        Compressor() =
        default;
    ~Compressor() {}

    void process_sample_stereo(float * in_sample_l, float * in_sample_r) {
        float smp = 0.5 f * * in_sample_l + 0.5 f * * in_sample_r;
        summ += smp * smp;
        float theta = rms > env ? att : rel;
        env = (1.0 - theta) * rms + theta * env;
        gain = exp(std::max < float > (0.0, log(env) - log(threshold)) * slope);
        //gain=1.0;

        if (gain > threshold) {
            gain = gain - (env - threshold) * slope;
        }

        reduction_level = +(env - threshold) * slope;

        * in_sample_l *= gain;
        * in_sample_r *= gain;

        rms_samples++;

        if (rms_samples > nrms) {
            rms = sqrt(summ / nrms);
            rms_samples = 0;
            summ = 0;
            env = 0;
        }

    }

    void update_compressor_threshold() {
        threshold *= 0.01; // threshold to unity (0...1)
    }

    void update_compressor_slope() {
        slope *= 0.01; // threshold to unity (0...1)
    }

    void update_compressor_attack() {
        tatt *= 1e-3; // attack time to seconds
        att = (tatt == 0.0) ? (0.0) : exp(-1.0 / (samplerate * tatt));

    }

    void update_compressor_release() {
        trel *= 1e-3; // release time to seconds
        rel = (trel == 0.0) ? (0.0) : exp(-1.0 / (samplerate * trel));

    }

    void update_compressor() {
        threshold *= 0.01; // threshold to unity (0...1)
        slope *= 0.01; // slope to unity
        tla *= 1e-3; // lookahead time to seconds
        twnd *= 1e-3; // window time to seconds
        tatt *= 1e-3; // attack time to seconds
        trel *= 1e-3; // release time to seconds
        att = (tatt == 0.0) ? (0.0) : exp(-1.0 / (samplerate * tatt));
        rel = (trel == 0.0) ? (0.0) : exp(-1.0 / (samplerate * trel));

    }

    void init_compressor() {
        update_compressor();
        lhsmp = (int)(samplerate * tla);
        // samples count in lookahead window
        nrms = (int)(samplerate * twnd) + lhsmp;
        env = 0.0;

    }

    void compress_process(float * buffer_in, int n, int bytes_per_sample) {
        //compress(buffer_in,n,threshold,slope,samplerate,lookahead_ms,window_ms,attack_ms,release_ms,bytes_per_sample);
    }

    //    float lookahead_ms=3.0;
    //    float window_ms=1.0;
    //    float attack_ms=0.10;
    //    float release_ms=0.300;

    int samplerate = 192000;
    int rms_samples = 0;
    int nrms = 0;
    float threshold = 0.5;
    float slope = 0.5;
    float reduction_level;
    float lhsmp = 0;
    float rms = 0;
    float env = 0.0;
    float gain = 1.0;
    float att = 0.0;
    float rel = 0.0;
    float tatt = 0.1;
    float trel = 0.3;
    float tla = 3;
    float twnd = 0.1;
    float summ = 0;

    //    float theta=0;
    private:

};

#endif
