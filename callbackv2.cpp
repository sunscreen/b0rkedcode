static void write_callback2(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max) {
 struct RecordContext *rc = (RecordContext*)outstream->userdata;

 struct SoundIoChannelArea *areas;
    int frames_left=0;
    int frame_count=0;
    int err=0;

    char *read_ptr = soundio_ring_buffer_read_ptr(rc->ring_buffer);
    int fill_bytes = soundio_ring_buffer_fill_count(rc->ring_buffer);
    int fill_count = fill_bytes / outstream->bytes_per_frame;

    if (frame_count_min > fill_count) {
        // Ring buffer does not have enough data, fill with zeroes.
        printf("Ring buffer to small %d - %d\n",frame_count_min,fill_count);
        frames_left = frame_count_min;
        for (;;) {
            frame_count = frames_left;
            if (frame_count <= 0)
              return;
            if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count)))
                printf("begin write error: %s\n", soundio_strerror(err));
            if (frame_count <= 0)
                return;
            for (int frame = 0; frame < frame_count; frame += 1) {
                for (int ch = 0; ch < outstream->layout.channel_count; ch += 1) {
                    memset(areas[ch].ptr, 0, outstream->bytes_per_sample);
                    areas[ch].ptr += areas[ch].step;
                }
            }
            if ((err = soundio_outstream_end_write(outstream)))
                printf("end write error: %s\n", soundio_strerror(err));
            frames_left -= frame_count;
        }
    }
    int frame_track=0;
    size_t recv_frames=0;

    int read_count = min_int(frame_count_max, fill_count);
    frames_left = read_count;
    //update_rds_buffers();

    while (frames_left > 0) {
        int frame_count = frames_left;
        //printf("processing %d frames\n",frames_left);
        src_data.input_frames = frame_count;
        src_data.output_frames = frame_count;


        fm_rds_get_frames(mpx_buffer, frame_count);
        int res_result=resample(src_state, src_data, &recv_frames);
        if (res_result != 0) { printf("res error %s\n",src_strerror (res_result));  }


        if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count)))
            printf("begin write error: %s\n", soundio_strerror(err));

        if (frame_count <= 0)
            break;

        for (int frame = 0; frame < frame_count; frame += 1,frame_track++) {

            for (int ch = 0; ch < outstream->layout.channel_count; ch += 1) {
                float read_samp=out_buffer[2*frame_track+ch];
                float mix_samp=0.0;
                memcpy(&mix_samp,read_ptr,outstream->bytes_per_sample);
                float mix_final = 0.6 * (mix_samp+read_samp);
                write_sample(areas[ch].ptr,mix_final);
                areas[ch].ptr += areas[ch].step;
                read_ptr += outstream->bytes_per_sample;
            }
            if (frame_track == recv_frames) { frame_track=0;}

        }

        if ((err = soundio_outstream_end_write(outstream)))
            printf("end write error: %s\n", soundio_strerror(err));

        frames_left -= frame_count;
    }

    soundio_ring_buffer_advance_read_ptr(rc->ring_buffer, read_count * outstream->bytes_per_frame);

    double sleeptime=0;
    soundio_outstream_get_latency(outstream,&sleeptime);
    int sleepa=sleeptime*100000000/1.2;
    std::this_thread::sleep_for(std::chrono::nanoseconds(sleepa));

}

