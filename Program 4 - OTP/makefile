key: keygen.c
	gcc -std=gnu11 -o keygen keygen.c
enc: otp_enc.c
	gcc -std=gnu11 -o otp_enc otp_enc.c
encd: otp_enc_d.c
	gcc -std=gnu11 -o otp_enc_d otp_enc_d.c
dec: otp_dec.c
	gcc -std=gnu11 -o otp_dec otp_dec.c
decd: otp_dec_d.c
	gcc -std=gnu11 -o otp_dec_d otp_dec_d.c
all: keygen.c otp_enc.c otp_enc_d.c otp_dec.c otp_dec_d.c
	gcc -std=gnu11 -o keygen keygen.c
	gcc -std=gnu11 -o otp_enc otp_enc.c
	gcc -std=gnu11 -o otp_enc_d otp_enc_d.c
	gcc -std=gnu11 -o otp_dec otp_dec.c
	gcc -std=gnu11 -o otp_dec_d otp_dec_d.c
clean:
	rm -rf *.o keygen client server otp_dec_d otp_dec otp_enc_d otp_enc
