#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

static int write_stdout(const char *token, int length) {
	int rc;
	int bytes_written = 0;

	do {
		rc = write(1, token + bytes_written, length - bytes_written);
		if (rc < 0)
			return rc;

		bytes_written += rc;
	} while (bytes_written < length);

	return bytes_written;
}

// functia returneaza numarul de cifre ale unui long dat
int get_digits(long n) {

	int num_digits = 0;

	if (n == 0) return 1;
	while (n) {
		n = n / 10;
		num_digits++;
	}

	return num_digits;
}

// functia returneaza numarul de cifre ale unui unsigned int
// in scrierea lui in hexa
int get_hex_digits(unsigned int n) {

	int num_digits = 0;

	if (n == 0) return 1;
	while (n) {
		n = n / 16;
		num_digits++;
	}

	return num_digits;
}


// functia converteste un long primit ca si parametru
// intr-un string
char* int_to_string(long n) {

	char* buffer;
	int i = 0, num_digits, j;
	long copy_n = n;
	char aux;

	// determin numarul de cifre din scrierea numarului
	num_digits = get_digits(n);

	if (n >= 0) {
		// +1 pentru NULL
		buffer = (char *) malloc(sizeof(char) * (num_digits + 1)); 
	}
	
	else {
		// +1 pentru NULL si +1 pentru "-"
		buffer = (char *) malloc(sizeof(char) * (num_digits + 2));
		n = -n;
	}
  
  	// construiesc string-ul cu cifrele in ordine inversa
	while (n != 0) {
		buffer[i] = n % 10 + '0';
		i++;
		n = n / 10;
	}

	if (copy_n < 0) {
		buffer[i] = '-';
		i++;
	}

	buffer[i] = '\0';

	// reverse string-ului
	for (j = 0; j < strlen(buffer) / 2; j++) {
		aux = buffer[j];
		buffer[j] = buffer[strlen(buffer) - j - 1];
		buffer[strlen(buffer) - j - 1] = aux;
	}

	// caz special daca numarul era chiar 0
	if (copy_n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
	}   

	return buffer;
}

// functia converteste din hexa in string
char* hex_to_string(unsigned int n){

	int i = 0, digits_hex_num, temp;
	char* hex_num;
	char aux;

	// determin cate cifre ar trebui sa aiba in scrierea
	// hexazecimala numarul dat
	digits_hex_num = get_hex_digits(n);
	hex_num = calloc((digits_hex_num + 1), sizeof(char));
	
	// construiesc numarul in hexa, dar cifrele lui
	// vor fi in ordine inversa initial
	while (n != 0) {
		temp = n % 16; 
		if (temp < 10) { 
			temp = temp + '0'; 
		} 
		else {
			temp = temp + 'a' - 10; 
		} 
		hex_num[i] = temp; 
		n = n / 16;
		i++;
	}

	// fac reverse pentru a aranja caracterele
	// in ordinea corecta 
	for (i = 0; i < digits_hex_num / 2; i++) {
		aux = hex_num[i];
		hex_num[i] = hex_num[digits_hex_num - i - 1];
		hex_num[digits_hex_num - i - 1] = aux;

	}

	return hex_num;
}

int iocla_printf(const char *format, ...) {

	/* TODO */
	int num_bytes = 0, i = 0, prev = 0, length = 0;

 	va_list args;
	va_start(args, format);

	// caut primul "%"
	while (format[i] != '%' && i < strlen(format)) {
			i++;
	}

	// printez ce e pana la primul "%"
	write_stdout(format, i);
	num_bytes = num_bytes + i;

	while (i < strlen(format)) {

		// pe pozitia format[i] se afla acum "%";
		if (i + 1 < strlen(format)){

			// daca am un specificator dupa "%" se va 
			// executa unul dintre if-uri si ok-ul
			// va avea valoarea 1, ulterior
			int ok = 0;
			if (format[i + 1] == 'd') {
				int argm = va_arg(args, int);
				char *buffer = int_to_string(argm);
				write_stdout(buffer, strlen(buffer));
				num_bytes = num_bytes + strlen(buffer);
				free(buffer);
				ok = 1;
			}
			if (format[i + 1] == 'u') {
				unsigned int argm = (unsigned int) va_arg(args, unsigned int);
				char *buffer = int_to_string(argm);
				write_stdout(buffer, strlen(buffer));
				num_bytes = num_bytes + strlen(buffer);
				free(buffer);
				ok = 1;
			}
			if (format[i + 1] == 'x') {
				unsigned int argm = (unsigned int) va_arg(args, unsigned int);
				char *buffer = hex_to_string(argm);
				write_stdout(buffer, strlen(buffer));
				num_bytes = num_bytes + strlen(buffer);
				free(buffer);
				ok = 1;
			}
			if (format[i + 1] == 'c') {
				char argm = (char) va_arg(args, int);
				write_stdout(&argm, 1);
				num_bytes++;
				ok = 1;
			}
			if (format[i + 1] == 's') {
				char* argm = (char*) va_arg(args, int*);
				write_stdout(argm, strlen(argm));
				num_bytes = num_bytes + strlen(argm);
				ok = 1;
			}
			// daca am "%%" se va printa doar "%"
			if (format[i + 1] == '%') {
				write_stdout("%",1);
				num_bytes++;
				ok = 1;
			}
			// daca a fost o comanda corecta
			if (ok == 1) {
				// sar peste "%" si specificator
				i += 2;
			}
			// functia nu a primit parametri corect
			else {
				return -1;
			}
		}
		// functia nu este apelata corect, "format" se termina
		// cu "%"
		else {
			return -1;
		}
		
		// pe pozitia prev este primul caracter nescris inca
		// de functie
		prev = i;
		// lenght va contine lungimea string-ului dintre 2 "%"
		// consecutivi
		length = 0;

		// caut urmatorul "%" din string
		while (format[i] != '%' && i < strlen(format)) {
			i++;
			length++;
		}

		// scriu string-ul de lungime length dintre cele 2 "%"
		write_stdout(format + prev, length);
		num_bytes = num_bytes + length;

	}

	return num_bytes;
	return -1;
}
