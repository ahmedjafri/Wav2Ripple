//
//  WavReader.cpp
//  RippleEngine
//
//  Created by Ahmed Jafri on 6/1/16.
//  Copyright © 2016 Ripple. All rights reserved.
//

#include "WavReader.h"
#include <exception>
#include <cstdlib>

unsigned char buffer4[4];
unsigned char buffer2[2];

char* seconds_to_time(float raw_seconds);

WavReader::WavReader(){
    
}

WavReader::WavReader(const char * pathToWavFile){
    this->file = pathToWavFile;

    FILE* fp = fopen(pathToWavFile,"r");
    if (fp == NULL) {
        printf("Error opening file\n");
        throw new std::exception();
        return;
    }

    int read = 0;
    
    // read header parts
    
    read = fread(this->header.riff, sizeof(this->header.riff), 1, fp);
    printf("(1-4): %s \n", this->header.riff);
    
    read = fread(buffer4, sizeof(buffer4), 1, fp);
    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
    
    // convert little endian to big endian 4 byte int
    this->header.overall_size  = buffer4[0] |
    (buffer4[1]<<8) |
    (buffer4[2]<<16) |
    (buffer4[3]<<24);
    
    printf("(5-8) Overall size: bytes:%u, Kb:%u \n", this->header.overall_size, this->header.overall_size/1024);
    
    read = fread(this->header.wave, sizeof(this->header.wave), 1, fp);
    printf("(9-12) Wave marker: %s\n", this->header.wave);
    
    read = fread(this->header.fmt_chunk_marker, sizeof(this->header.fmt_chunk_marker), 1, fp);
    printf("(13-16) Fmt marker: %s\n", this->header.fmt_chunk_marker);
    
    read = fread(buffer4, sizeof(buffer4), 1, fp);
    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
    
    // convert little endian to big endian 4 byte integer
    this->header.length_of_fmt = buffer4[0] |
    (buffer4[1] << 8) |
    (buffer4[2] << 16) |
    (buffer4[3] << 24);
    printf("(17-20) Length of Fmt this->header: %u \n", this->header.length_of_fmt);
    
    read = fread(buffer2, sizeof(buffer2), 1, fp); printf("%u %u \n", buffer2[0], buffer2[1]);
    
    this->header.format_type = buffer2[0] | (buffer2[1] << 8);
    char format_name[10] = "";
    if (this->header.format_type == 1)
        strcpy(format_name,"PCM");
    else if (this->header.format_type == 6)
        strcpy(format_name, "A-law");
    else if (this->header.format_type == 7)
        strcpy(format_name, "Mu-law");
    
    printf("(21-22) Format type: %u %s \n", this->header.format_type, format_name);
    
    read = fread(buffer2, sizeof(buffer2), 1, fp);
    printf("%u %u \n", buffer2[0], buffer2[1]);
    
    this->header.channels = buffer2[0] | (buffer2[1] << 8);
    printf("(23-24) Channels: %u \n", this->header.channels);
    
    read = fread(buffer4, sizeof(buffer4), 1, fp);
    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
    
    this->header.sample_rate = buffer4[0] |
    (buffer4[1] << 8) |
    (buffer4[2] << 16) |
    (buffer4[3] << 24);
    
    printf("(25-28) Sample rate: %u\n", this->header.sample_rate);
    
    read = fread(buffer4, sizeof(buffer4), 1, fp);
    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
    
    this->header.byterate  = buffer4[0] |
    (buffer4[1] << 8) |
    (buffer4[2] << 16) |
    (buffer4[3] << 24);
    printf("(29-32) Byte Rate: %u , Bit Rate:%u\n", this->header.byterate, this->header.byterate*8);
    
    read = fread(buffer2, sizeof(buffer2), 1, fp);
    printf("%u %u \n", buffer2[0], buffer2[1]);
    
    this->header.block_align = buffer2[0] |
    (buffer2[1] << 8);
    printf("(33-34) Block Alignment: %u \n", this->header.block_align);
    
    read = fread(buffer2, sizeof(buffer2), 1, fp);
    printf("%u %u \n", buffer2[0], buffer2[1]);
    
    this->header.bits_per_sample = buffer2[0] |
    (buffer2[1] << 8);
    printf("(35-36) Bits per sample: %u \n", this->header.bits_per_sample);
    
    // since there seems to be a LIST subchunk at this location, skip to the beginning of data
    int skipAmount = 34;
    fseek(fp, skipAmount * sizeof(char), SEEK_CUR);

    read = fread(this->header.data_chunk_header, sizeof(this->header.data_chunk_header), 1, fp);
    printf("(37-40) Data Marker: %s \n", this->header.data_chunk_header);
    
    read = fread(buffer4, sizeof(buffer4), 1, fp);
    printf("%u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
    
    this->header.data_size = buffer4[0] |
                (buffer4[1] << 8) |
                (buffer4[2] << 16) |
                (buffer4[3] << 24 );
    printf("(41-44) Size of data chunk: %u \n", this->header.data_size);
    
    
    // calculate no.of samples
    num_samples = (8 * this->header.data_size) / (this->header.channels * this->header.bits_per_sample);
    printf("Number of samples:%lu \n", num_samples);
    
    size_of_each_sample = (this->header.channels * this->header.bits_per_sample) / 8;
    printf("Size of each sample:%ld bytes\n", size_of_each_sample);
    
    // calculate duration of file
    duration_in_seconds = (float) this->header.overall_size / this->header.byterate;
    printf("Approx.Duration in seconds=%f\n", duration_in_seconds);
    printf("Approx.Duration in h:m:s=%s\n", seconds_to_time(duration_in_seconds));

    headerOffset = ftell(fp);
    
    fclose(fp);
}

std::vector<float> WavReader::read(int start, int length) {
    FILE* fileHandle = fopen(file,"r");
    
    float* output = (float*)malloc(sizeof(float)*length);

    std::vector<float> samples = std::vector<float>(length);
    
    // read each sample from data chunk if PCM
    if (this->header.format_type == 1) { // PCM
            long i =0;
            char data_buffer[size_of_each_sample];
            bool  size_is_correct = true;
            
            // make sure that the bytes-per-sample is completely divisible by num.of channels
            long bytes_in_each_channel = (size_of_each_sample / this->header.channels);
            if ((bytes_in_each_channel  * this->header.channels) != size_of_each_sample) {
                printf("Error: %ld x %ud <> %ld\n", bytes_in_each_channel, this->header.channels, size_of_each_sample);
                size_is_correct = false;
            }
            
            if (size_is_correct) {
                // the valid amplitude range for values based on the bits per sample
                long low_limit = 0l;
                long high_limit = 0l;
                
                switch (this->header.bits_per_sample) {
                    case 8:
                        low_limit = -128;
                        high_limit = 127;
                        break;
                    case 16:
                        low_limit = -32768;
                        high_limit = 32767;
                        break;
                    case 32:
                        low_limit = -2147483648;
                        high_limit = 2147483647;
                        break;
                }
                
                printf("\n\n.Valid range for data values : %ld to %ld \n", low_limit, high_limit);

                int bytes_per_sample = this->header.bits_per_sample/8;
                fseek(fileHandle, headerOffset + bytes_per_sample*start, SEEK_SET);
                int read = 0;
                
                for (i = 0; i <= length; i++) {
                    read = fread(data_buffer, sizeof(data_buffer), 1, fileHandle);
                    if (read == 1) {
                        // dump the data read
                        unsigned int  xchannels = 0;
                        int data_in_channel = 0;
                        
                        float average = 0;
                        for (xchannels = 0; xchannels < this->header.channels; xchannels ++ ) {
                            // convert data from little endian to big endian based on bytes in each channel sample
                            if (bytes_in_each_channel == 4) {
                                data_in_channel =   data_buffer[0] |
                                (data_buffer[1]<<8) |
                                (data_buffer[2]<<16) |
                                (data_buffer[3]<<24);
                            }
                            else if (bytes_in_each_channel == 2) {
                                data_in_channel = data_buffer[0] |
                                (data_buffer[1] << 8);
                            }
                            else if (bytes_in_each_channel == 1) {
                                data_in_channel = data_buffer[0];
                            }
                            
                            average += data_in_channel;
                        }
                        average = average / this->header.channels;
                        samples[i] = average / high_limit;
                    }
                    else {
                        printf("Error reading file. %d bytes\n", read);
                        break;
                    }
                    
                } //    for (i =1; i <= num_samples; i++) {
                
            } //    if (size_is_correct) { 
            
    } //  if (header.format_type == 1) {
    
    fclose(fileHandle);

    return samples;
}

long WavReader::sampleRate() {
    return 44100;
}

int WavReader::numberOfSamples() {
    FILE* fileHandle = fopen(file,"r");
    
    fseek(fileHandle, 0, SEEK_END);
    long fileSizeBytes = ftell(fileHandle) - sizeof(long long);
    
    fclose(fileHandle);
    
    return (int)fileSizeBytes/sizeof(float);
}

/**
 * Convert seconds into hh:mm:ss format
 * Params:
 *  seconds - seconds value
 * Returns: hms - formatted string
 **/
char* seconds_to_time(float raw_seconds) {
    char *hms;
    int hours, hours_residue, minutes, seconds, milliseconds;
    hms = (char*) malloc(100);
    
    sprintf(hms, "%f", raw_seconds);
    
    hours = (int) raw_seconds/3600;
    hours_residue = (int) raw_seconds % 3600;
    minutes = hours_residue/60;
    seconds = hours_residue % 60;
    milliseconds = 0;
    
    // get the decimal part of raw_seconds to get milliseconds
    char *pos;
    pos = strchr(hms, '.');
    int ipos = (int) (pos - hms);
    char decimalpart[15];
    memset(decimalpart, ' ', sizeof(decimalpart));
    strncpy(decimalpart, &hms[ipos+1], 3);
    milliseconds = atoi(decimalpart);   
    
    
    sprintf(hms, "%d:%d:%d.%d", hours, minutes, seconds, milliseconds);
    return hms;
}