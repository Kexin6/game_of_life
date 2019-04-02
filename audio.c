//
// Created by Kexin Li on 2019-04-02.
//



volatile int * audio_ptr = (int *) 0XFF203040;
int fifospace, int buffer_index = 0;
int left_buffer[BUF_SIZE];
int right_buffer[BUF_SIZE];

fifospace = *(audio_ptr + 1);
if ((fifospace & 0x000000FF) > 96) {
/* store data until the audio-in FIFO is empty or the memory buffer is full */
    while ( (fifospace & 0x000000FF) && (buffer_index < BUF_SIZE) ) {
        left_buffer[buffer_index] = *(audio_ptr + 2);
        right_buffer[buffer_index] = *(audio_ptr + 3);
        ++ buffer_index;
        fifospace = *(audio_ptr + 1);
    }
}