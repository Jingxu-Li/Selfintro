#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#define FMSG 100.0 // in kHz
#define FC 1000.0 // in kHz
#define FS 10000.0 // in kHz
// the number of samples in the carrier for 2 bit in the message is
#define NS 2*FS/FMSG
//simple random number from normal distribution (Knuth)
float randn(void) {
double v1,v2,s;
do {
v1 = 2.0 * ((double) rand()/RAND_MAX) - 1;
v2 = 2.0 * ((double) rand()/RAND_MAX) - 1;
s = v1*v1 + v2*v2;
} while ( s >= 1.0 );
if (s == 0.0)
return (float)0.0;
else
return (float)(v1*sqrt(-2.0 * log(s) / s));
}
int main(int argc, char *argv[]){
FILE *fin, *fout;
float noise_rms, m[4];
float *carrier, *out;
uint8_t in;
size_t n,i;
// check if we have 3 arguments - remember that the first one
is the file name
if(argc!=4){
printf("ERROR: not enough input parameters\n");
printf("USE: %s input_file rms_noise
output_file\n",argv[0]);
exit(1);
}
// open input files as binary read-only
fin=fopen(argv[1],"rb");
if(fin == NULL) {
printf("ERROR: %s does not exist\n",argv[1]);
exit(1);
}
// open output files as binary - overwrite the file if it
alredy exists
fout=fopen(argv[3],"w+b");
if(fout == NULL) {
printf("ERROR: %s cannot be created\n",argv[3]);
exit(1);
}
// get the noise rms
noise_rms = atof(argv[2]);
out = malloc(NS*sizeof(float));
carrier = malloc(NS*sizeof(float));
if ((carrier == NULL) || (out == NULL)) {
printf("ERROR: cannot allocate enough memory\n");
exit(1);
}
// build the sinusoidal carrier (it does not change)
for(n=0;n<NS;n++){
carrier[n] = cos(2*M_PI*n*FC/FS);
}
printf("processing ...\n");
// keep reading one byte from the input file until the end
while(fread(&in, sizeof(uint8_t), 1, fin)){
// from byte in, read two bits at the time (from the
MSB)
m[0] = (float)((in & 0xC0u)>>6)*0.2+0.1;
m[1] = (float)((in & 0x30u)>>4)*0.2+0.1;
m[2] = (float)((in & 0x0Cu)>>2)*0.2+0.1;
m[3] = (float)(in & 0x03u)*0.2+0.1;
for(i=0;i<4;i++){
for(n=0;n<NS;n++){
out[n] =
m[i]*carrier[n]+noise_rms*randn();
}
fwrite(&out, sizeof(float), NS, fout);
}
}
printf("done\n");
// close the files
fclose(fin);
fclose(fout);
exit(0); // all is good
}