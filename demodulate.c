#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define FMSG 100.0 // in kHz
#define FC 1000.0 // in kHz
#define FS 10000.0 // in kHz
#define NS 2*FS/FMSG
#define M_PI 3.1415926
#define FRAME_READ_LENGTH   NS
#define FIR_FILTER_LENGTH 22
#define length 200
//Filter: 2*Fc: 2MHz
const float filter[22] = {
   0.003157782567728, 0.006338278253387,  0.01178863668977,  0.01925699190672,
	0.02858870102633,  0.03931385099662,  0.05065614348845,  0.06161624168445,
	0.07110650872751,  0.07810571806971,  0.08182123618519,  0.08182123618519,
	0.07810571806971,  0.07110650872751,  0.06161624168445,  0.05065614348845,
	0.03931385099662,  0.02858870102633,  0.01925699190672,  0.01178863668977,
   0.006338278253387, 0.003157782567728
};
float state[FIR_FILTER_LENGTH + 1] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

void process(float* x, float* y, int N)
{
	//float* carrier = (float*)malloc(N*sizeof(float));
	for (size_t n = 0; n < N; n++) {
		//carrier[n] = cos(2 * M_PI * n * FC / FS);
		x[n]*= 2*cos(2 * M_PI * n * FC / FS);
	}
	int i = 0, j = 0, k = 0;
	float temp;
	for (k = 0; k < N; k++)
	{
		state[0] = x[k];
		for (i = 0, temp = 0; i < FIR_FILTER_LENGTH; i++)
			temp += filter[i] * state[i];
		y[k] = temp;
		for (j = FIR_FILTER_LENGTH - 1; j > -1; j--)
			state[j + 1] = state[j];
	}
}
void demodulate(FILE* fin,FILE* fout) {
	printf("demodulating...\n");
	FILE* fin = fopen("D:/DSP/in.dat", "rb");
	FILE* fout = fopen("D:/DSP/out.dat", "wb");
	float* in = (float*)malloc(4*NS * sizeof(float));
	float* out = (float*)malloc(4*NS * sizeof(float));
	bool flag = false;
	while (fread(in, sizeof(float), 4*NS, fin)) {
		process(in, out, 4*NS);
		char res = 0;
		for (int i = 0; i < 4; i++) {
			float sum = 0;
			int base = NS * i;		
			for (int j = 0; j < NS; j++) {
				sum += out[base + j];				
			}
			sum = ((sum / (NS))-0.1)*5;
			char floor = (int)(sum + 0.5) > (int)(sum) ? (char)(sum + 1) : (char)(sum);
			res += floor << (6 - 2 * i);	
		}
		fwrite(&res, sizeof(char), 1, fout);
	}

}

int main(int argc,char *argv[]) {
    if(argc!=3){
        printf("ERROR: not enough input parameters\n");
        printf("USE: %s input_file rms_noise output_file\n",argv[0]);
        exit(1);
    }
    
    // open input files as binary read-only
    fin=fopen(argv[1],"rb");
    if(fin == NULL) {
    printf("ERROR: %s does not exist\n",argv[1]);
        exit(1);
    }
    
    // open output files as binary - overwrite the file if it already exists
    fout=fopen(argv[2],"w+b");
    if(fout == NULL) {
        printf("ERROR: %s cannot be created\n",argv[2]);
        exit(1);
    }
    
	demodulate(fin,fout);
    fclose(fin);
	fclose(fout);
	return 0;
}



