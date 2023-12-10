#include <stdio.h>
#include <string.h>
#define __USE_XOPEN
#include <time.h>
#include <math.h>

const char  *okul_kapali = "Okul kapalı",
	    *tenefuse_kaldi = "Tenefüsün bitmesine %d dakika kaldı",
	    *ogle_arasinin_bitmesine = "Öğle arasının bitmesine %d dakika kaldı",
	    *derse_kaldi = "Dersin bitmesine %d dakika kaldı",
	    *dersin_kaldi = "Dersin başlamasına %d dakika kaldı",
	    *okulun_bitmesine = "Okulun bitmesine %d dakika kaldı";
	    
const char* tum_karakterler = "1234567890qwertyuıopğüasdfghjklşizxcvbnmöçQWERTYUIOPĞÜASDFGHJKLŞİZXCVBNMÖÇ";


const int ders_suresi = 40, tenefus_suresi = 10,
	  tenefus_suresi_cuma = 5,
	  okulun_baslmasi = 60*8 + 15,
	  okulun_bitmesi = 60*15 + 20,
	  okulun_bitmesi_cuma = 60*15 + 15,
	  ogle_arasinin_baslamasi = (ders_suresi+tenefus_suresi)*5+ders_suresi,
	  ogle_arasinin_baslamasi_cuma = ders_suresi+tenefus_suresi+((ders_suresi+tenefus_suresi_cuma)*5)-tenefus_suresi_cuma,
	  ogle_arasi = 45,
	  ogle_arasi_cuma = 60;


void get_relative_time(struct tm tms, char* relative) {
	if(tms.tm_wday == 6 || tms.tm_wday == 0) { // Haftasonu
		strcpy(relative, okul_kapali);
		return;
	}
	//char buffer[64] = {0};
	int geceden_beri = tms.tm_hour * 60 + tms.tm_min;
	int okuldan_beri = geceden_beri - okulun_baslmasi;
	if(okuldan_beri < 0) {
		sprintf(relative, dersin_kaldi, -okuldan_beri);
		return;
	}
	if(tms.tm_wday == 5) { // Cuma
		if(geceden_beri - okulun_bitmesi_cuma > 0) {
			strcpy(relative, okul_kapali);
			return;
		}
		if(okuldan_beri < ogle_arasinin_baslamasi_cuma) { // Öğle arasından önce
			if(okuldan_beri < 50) {
				int kalan = okuldan_beri % 50;
				if(kalan < 40) { // dersin bitmesine
					sprintf(relative, derse_kaldi, ders_suresi-kalan);
					return;
				}else { // Tenefüsün bitmesine
					sprintf(relative, tenefuse_kaldi, tenefus_suresi-(kalan-ders_suresi));
					return;
				}
			}else {
				int kalan = (okuldan_beri-50) % (ders_suresi+tenefus_suresi_cuma);
				if(kalan < 40) { // dersin bitmesine
					sprintf(relative, derse_kaldi, ders_suresi-kalan);
					return;
				}else { // Tenefüsün bitmesine
					sprintf(relative, tenefuse_kaldi, tenefus_suresi_cuma-(kalan-ders_suresi));
					return;
				}
			}
		}
		int ogleden_beri = okuldan_beri - ogle_arasinin_baslamasi_cuma;
		if(ogleden_beri < ogle_arasi_cuma) { // Hala öğle arası
			sprintf(relative, ogle_arasinin_bitmesine, ogle_arasi_cuma - ogleden_beri);
			return;
		}
		int ogleden_sonra = ogleden_beri - ogle_arasi_cuma;
		if(ogleden_sonra < 40) { // 7. ders içinde
			sprintf(relative, derse_kaldi, ders_suresi - ogleden_sonra);
			return;
		}else if(ogleden_sonra < ders_suresi+tenefus_suresi) { // Son tenefüs içinde
			sprintf(relative, tenefuse_kaldi, tenefus_suresi-(ogleden_sonra-ders_suresi));
			return;
		}else { // Okul bitişi
			sprintf(relative, okulun_bitmesine, ders_suresi-(ogleden_sonra-(ders_suresi+tenefus_suresi)));
			return;
		}
		return;
	}else { // Diğer günler
		if(geceden_beri - okulun_bitmesi > 0) {
			strcpy(relative, okul_kapali);
			return;
		}
		//int ders = ceil(okuldan_beri / 50.0);
		if(okuldan_beri < ogle_arasinin_baslamasi) { // Öğle arasından önce
			int kalan = okuldan_beri % 50;
			if(kalan < 40) { // dersin bitmesine
				sprintf(relative, derse_kaldi, ders_suresi-kalan);
				return;
			}else { // Tenefüsün bitmesine
				sprintf(relative, tenefuse_kaldi, tenefus_suresi-(kalan-ders_suresi));
				return;
			}
		}
		int ogleden_beri = okuldan_beri - ogle_arasinin_baslamasi;
		if(ogleden_beri < ogle_arasi) { // Hala öğle arası
			sprintf(relative, ogle_arasinin_bitmesine, ogle_arasi - ogleden_beri);
			return;
		}
		int ogleden_sonra = ogleden_beri - ogle_arasi;
		if(ogleden_sonra < 40) { // 7. ders içinde
			sprintf(relative, derse_kaldi, ders_suresi - ogleden_sonra);
			return;
		}else if(ogleden_sonra < 50) { // Son tenefüs içinde
			sprintf(relative, tenefuse_kaldi, tenefus_suresi-(ogleden_sonra-ders_suresi));
			return;
		}else { // Okul bitişi
			sprintf(relative, okulun_bitmesine, ders_suresi-(ogleden_sonra-(ders_suresi+tenefus_suresi)));
			return;
		}
	}
}

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("ERROR: please give time\n");
		return -1;
	}
	struct tm time_s;
	strptime(argv[1], "%w/%H:%M", &time_s);
	char buff[64] = {0};
	get_relative_time(time_s, buff);
	printf("%s\n", buff);
	return 0;
}
