
static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max) {
    //const struct SoundIoChannelLayout *layout = &outstream->layout;

    struct SoundIoChannelArea *areas;
    int err=0;
    size_t recv_frames;
    float sample1=0.0f;
    float sample2=0.0f;
    int frames_left = frame_count_max;

        if (!frames_left)
            return;

        size_t frame_count = frames_left;
        global_frame_count=frame_count;
        src_data.input_frames = frame_count;
        src_data.output_frames = frame_count;
        fm_rds_get_frames(mpx_buffer, frame_count);
        int res_result=resample(src_state, src_data, &recv_frames);
        if (res_result != 0) { printf("res error %s\n",src_strerror (res_result));  }

        if ((err = soundio_outstream_begin_write(outstream, &areas, (int*)&recv_frames))) {
            fprintf(stderr, "unrecoverable stream error (write): %s\n", soundio_strerror(err));
            exit(1);
        }

        if (recv_frames != frame_count) { printf("resampler weakness\n"); return; }
//      printf("want to process %d src in: %d src out: %d\n",frame_count,src_data.input_frames,src_data.output_frames);

        for (size_t frame = 0; frame < recv_frames; frame += 1) {
            sample1 = out_buffer[2*frame+0];
            sample2 = out_buffer[2*frame+1];

            decl++;

            if(!(decl%=1250))//about 154 times a second
            {
            oval=sample1;

            outbigval=fabs(oval);
            if(fabs(sample1)>outbigval)outbigval=fabs(oval);
            sigstats.outvol=oPeak.update(outbigval);
            decl=0;
            }

            write_sample(areas[0].ptr, sample1);
            write_sample(areas[1].ptr, sample2);
            areas[0].ptr += areas[0].step;
            areas[1].ptr += areas[1].step;

            sigstats.samples[sample_counter]=sample1;
            sample_counter++;

            if (sample_counter >= 2048) {sample_counter=0;}
            sigstats.sample_counter=sample_counter;

        }

//        seconds_offset = fmod(seconds_offset + seconds_per_frame * frame_count, 1.0);
        if ((err = soundio_outstream_end_write(outstream))) {
            if (err == SoundIoErrorUnderflow) return; /* we allow underflow*/
            fprintf(stderr, "unrecoverable stream error (write_end): %s\n", soundio_strerror(err));
            exit(1);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

//    soundio_outstream_pause(outstream, want_pause);
}
