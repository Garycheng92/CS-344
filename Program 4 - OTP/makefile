key: keygen.c
	gcc -o keygen keygen.c
client: client.c
	gcc -o client client.c
server: server.c
	gcc -o server server.c
enc: otp_enc.c
	gcc -o otp_enc otp_enc.c
encd: otp_enc_d.c
	gcc -o otp_enc_d otp_enc_d.c
dec: otp_dec.c
	gcc -o otp_dec otp_dec.c
decd: otp_dec_d.c
	gcc -o otp_dec_d otp_dec_d.c
all: keygen.c server.c client.c
	gcc -o keygen keygen.c
	gcc -o client client.c
	gcc -o server server.c
	gcc -o otp_enc otp_enc.c
	gcc -o opt_enc_d opt_enc_d.c
	gcc -o otp_dec otp_dec.c
	gcc -o otp_dec_d otp_dec_d.c
clean:
	rm -rf *.o keygen client server otp_dec_d otp_dec otp_enc_d otp_enc
