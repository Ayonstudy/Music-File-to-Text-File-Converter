#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <windows.h>

// Wave File to Text Convertor (WFTC)
// Made by Ayonstudy
// Project start date: 25th August    2024 01:11AM (GMT+6)
// Project end date:    6th September 2024 03:24PM (GMT+6)
// This executable only works with WAV files
// This executable converts WAV file data into text format
// The raw music data will be converted to integer format
// Compile with gcc: gcc -o WFTC WFTC.c
// Format for use: WFTC [Wav file to convert] [optional: name to be given for text file]

typedef struct {
	uint32_t samples;
	int16_t *data;
} sound_t;

bool LoadWav(const char *filename, sound_t *sound, const char *finaltext) {
	bool return_value = true;
	FILE *file;
	char text[4];
	int32_t filesize;
	int32_t format_length;
	int16_t format_type;
	int16_t num_channels;
	int32_t sample_rate;
	int32_t bytes_per_second;	                                                   // sample_rate * num_channels * bits_per_sample / 8
	int16_t block_align;		                                                   // num_channels * bits_per_sample / 8
	int16_t bits_per_sample;	                                                   // 16 bits
	int32_t data_size;

	// Reading and checking File Data
	file = fopen(filename, "rb");
	if(file == NULL) {
		printf("%s: Failed to open file", filename);
		return false;
	}
	fread(text, 1, 4, file);
	if(text[0] != 'R' || text[1] != 'I' || text[2] != 'F' || text[3] != 'F') {
		printf("It is not a WAV File");
		return_value = false;
		goto CLOSE_FILE;
	}
	fread(&filesize, 4, 1, file);
	fread(text, 1, 4, file);
	if(text[0] != 'W' || text[1] != 'A' || text[2] != 'V' || text[3] != 'E') {
		printf("%s 4 bytes should be \"WAVE\", are \"%4s\"", filename, text);
		return_value = false;
		goto CLOSE_FILE;
	}
	fread(text, 1, 4, file);
	if(text[0] != 'f' || text[1] != 'm' || text[2] != 't' || text[3] != ' ') {
		printf("%s 4 bytes should be \"fmt/0\", are \"%4s\"", filename, text);
		return_value = false;
		goto CLOSE_FILE;
	}
	fread(&format_length, 4, 1, file);
	fread(&format_type, 2, 1, file);
	fread(&num_channels, 2, 1, file);
	fread(&sample_rate, 4, 1, file);
	fread(&bytes_per_second, 4, 1, file);
	fread(&block_align, 2, 1, file);
	fread(&bits_per_sample, 2, 1, file);
	if(bits_per_sample != 16) {
		printf("%s bits per sample should be 16, is %d", filename, bits_per_sample);
		return_value = false;
		goto CLOSE_FILE;
	}
	fread(text, 1, 4, file);
	if(text[0] != 'd' || text[1] != 'a' || text[2] != 't' || text[3] != 'a') {
		printf("%s 4 bytes should be \"data\", are \"%4s\"", filename, text);
		return_value = false;
		goto CLOSE_FILE;
	}
	fread(&data_size, 4, 1, file);

	sound->data = malloc(data_size);
	if(sound->data == NULL) {
		printf("%s Failed to allocate %d bytes for data", filename, data_size);
		return_value = false;
		goto CLOSE_FILE;
	}

	if(fread(sound->data, 1, data_size, file) != data_size) {
		printf("%s Failed to read data bytes", filename);
		return_value = false;
		free(sound->data);
		goto CLOSE_FILE;
	}

	// Saving onto text file
	int percentage=0;
	FILE *fptr;
	fptr = fopen(finaltext, "a");
	fprintf(fptr,"RIFF,");
	fprintf(fptr,"%i,",filesize);
	fprintf(fptr,"WAVE,");
	fprintf(fptr,"fmt ,"); 
	fprintf(fptr,"%i,",format_length);                                             
	fprintf(fptr,"%i,",format_type);
	fprintf(fptr,"%i,",num_channels);
	fprintf(fptr,"%i,",sample_rate);
	fprintf(fptr,"%i,",bytes_per_second);
	fprintf(fptr,"%i,",block_align);
	fprintf(fptr,"%i,",bits_per_sample);
	fprintf(fptr,"data,");
	fprintf(fptr,"%i\n",data_size);
	for(int i=0;i<(data_size/2);i++){
		fprintf(fptr,"%i,", sound->data[i]);                                       // Raw data starts to get saved from here
		percentage=((i*200)/(data_size));
		printf("Progress: %i%c\r",percentage,'%');
	}
	printf("Progress: 100%c\n",'%');
	printf("Process Complete :D");
	fclose(fptr); 
	


	sound->samples = data_size / 2;

	CLOSE_FILE:
	fclose(file);
	free(sound->data);

	return return_value;
}

int main(int argc, char *argv[]) {

	// No parameter given
	if(argc<=1){
		printf("Too few arguements!\n");
		printf("Missing WAV file name.\n");
		printf("Type \"WTFC -h\" for help");
		return 1;
	}

	// First Parameter
	char *arg = argv[1];
	int j=0;
	while(arg[j]!=0){
		arg[j] = toupper(arg[j]);
		j++;
	}

	if(argc==2 && ((strcmp(arg,"-HELP")==0 || strcmp(arg, "--HELP")==0 || strcmp(arg, "/?")==0 || strcmp(arg, "-H")==0 || strcmp(arg, "--H")==0 )){
			printf("Converts WAV file data into text form.\n\n");
			printf("format for help: WFTC -h\n\n");
			printf("Format: WFTC [Wav file to convert] [optional: name to be given for text file\n\n");
	} else if(argc==2) {
		FILE *fptr;
		fptr = fopen("WAV_Data.txt", "w");
		fprintf(fptr,"");                                                          // Wiping previous file data
		fclose(fptr); 

		sound_t sound;
		if(!LoadWav(argv[1], &sound, "WAV_Data.txt")) {
			printf("Failed to load %s\n",argv[1]);
			return 1;
		}
	// Second parameter
	} else if (argc==3) {
		FILE *fptr;
		fptr = fopen(argv[2], "w");
		fprintf(fptr,"");                                                          // Wiping previous file data
		fclose(fptr); 

		sound_t sound;
		if(!LoadWav(argv[1], &sound, argv[2])) {
			printf("Failed to load %s\n",argv[1]);
			return 1;
		}
	// Third and more parameters
	} else if (argc>=4){
		printf("Too many arguements!\n");
		printf("Type \"WTFC -h\" for help");
		return 1;
	}
	return 0;
}
