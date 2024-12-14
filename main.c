#include "libbladeRF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// bandhwidht max 56Mhz
// glonas dahil için gerekli +-35MHZ : test edilecek


#define TX_BUFFER_SIZE	(4096) 
#define RX_BUFFER_SIZE	(4096) 

#define TX_FREQ			(1575420000)	//Hz
#define TX_SAMPLE_RATE	(8000000)		//Hz
#define TX_BANDWIDTH    (5000000)		//Hz
#define TX_GAIN		    (60)			//Db

#define RX_FREQ		    (1575420000)	//Hz
#define RX_SAMPLE_RATE  (8000000)		//Hz
#define RX_BANDWIDTH    (5000000)		//Hz
#define RX_GAIN			(60)			//Db


#define DURUM_KONTROL(aksiyon, mesaj) \
    do { \
        int durum = (aksiyon); \
        if (durum != 0) { \
            fprintf(stderr, "Hata: %s -> %s\n", (mesaj), bladerf_strerror(durum)); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)



typedef struct kanal_st
{
	bladerf_channel         channel;
	bladerf_sample_rate		sample_rate;
	bladerf_frequency		frequency;
	bladerf_gain		    gain;
	bladerf_bandwidth		bandwidth;
}kanal_st;



void kanal_baslat(struct bladerf* cihaz, kanal_st* kanal)
{
	DURUM_KONTROL(
		bladerf_set_sample_rate(cihaz, kanal->channel, kanal->sample_rate, NULL),
		"TX ornekleme hizi ayarlanmadi ! "
	);
	DURUM_KONTROL(
		bladerf_set_frequency(cihaz, kanal->channel, kanal->frequency),
		"TX frekansi ayarlanmadi ! "
	);
	DURUM_KONTROL(
		bladerf_set_gain(cihaz, kanal->channel, kanal->gain),
		"TX kazanci ayarlanmadi ! "
	);
	DURUM_KONTROL(
		bladerf_set_bandwidth(cihaz, kanal->channel, kanal->bandwidth, 0),
		"TX kazanci ayarlanmadi ! "
	);
	printf("Kanal (%s) basariyla baslatildi.\n", (kanal->channel == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX");
}

void kanal_bilgi_yazdir(struct bladerf* cihaz, kanal_st* kanal)
{
	bladerf_sample_rate sample_rate;
	bladerf_frequency frequency;
	bladerf_gain gain;
	bladerf_bandwidth bandwidth;

	// Ornekleme hýzý sorgula
	DURUM_KONTROL(bladerf_get_sample_rate(cihaz, kanal->channel, &sample_rate), "Ornekleme hizi alinamadi");
	printf("Kanal: %s - Ornekleme Hizi: %d Hz\n", (kanal->channel == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX", sample_rate);

	// Frekans sorgula
	DURUM_KONTROL(bladerf_get_frequency(cihaz, kanal->channel, &frequency), "Frekans alinamadi");
	printf("Kanal: %s - Frekans: %l Hz\n", (kanal->channel == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX", frequency);

	// Kazanç sorgula
	DURUM_KONTROL(bladerf_get_gain(cihaz, kanal->channel, &gain), "Kazanc alinamadi");
	printf("Kanal: %s - Kazanc: %d dB\n", (kanal->channel == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX", gain);

	// Bant Geniþliði sorgula
	DURUM_KONTROL(bladerf_get_bandwidth(cihaz, kanal->channel, &bandwidth), "Bant Genisligi alinamadi");
	printf("Kanal: %s - Bant Genisligi: %d Hz\n", (kanal->channel == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX", bandwidth);
}

int main() {

	struct bladerf_devinfo* cihaz_listesi_st	= NULL;
	struct bladerf*			cihaz_st			= NULL;
	int16_t					cihaz_sayisi_i16	= 0;
	kanal_st tx_kanal = { 0 };
	kanal_st rx_kanal = { 0 };
	

	/*-------------------------------------------Bagli Cihazlari Kontrol Et--------------------------------------------*/
	
	cihaz_sayisi_i16 = bladerf_get_device_list(&cihaz_listesi_st);
	if (cihaz_sayisi_i16 < 0) {
		fprintf(stderr, "Cihaz listesi alinamadi: %s\n", bladerf_strerror(cihaz_sayisi_i16));
		return -1;
	}

	printf("Bulunan cihazlar:\n");
	for (int i = 0; i < cihaz_sayisi_i16; i++) {
		printf("Cihaz %d: Cihaz Model: %s, Seri No: %s\n", i,cihaz_listesi_st[i].product, cihaz_listesi_st[i].serial);
	
	}

	/*--------------------------------------------Cihazi Ac-----------------------------------------------------------*/
	
	DURUM_KONTROL(
		bladerf_open_with_devinfo(&cihaz_st, &cihaz_listesi_st[0]),
		"Cihaz acilamadi!!"
	);
	printf("Cihaz basariyla acildi!\n");


	/*--------------------------------------------Cihaz Kanal Parametrelerini Kur ve Aktiflestir--------------------------------------------*/

	tx_kanal.channel		= BLADERF_CHANNEL_TX(0);
	tx_kanal.sample_rate	= TX_SAMPLE_RATE;
	tx_kanal.frequency		= TX_FREQ;
	tx_kanal.gain			= TX_GAIN;
	tx_kanal.bandwidth		= TX_BANDWIDTH;
	

	rx_kanal.channel		= BLADERF_CHANNEL_RX(0);
	rx_kanal.sample_rate	= RX_SAMPLE_RATE;
	rx_kanal.frequency		= RX_FREQ;
	rx_kanal.gain			= RX_GAIN;
	rx_kanal.bandwidth		= RX_BANDWIDTH;

	kanal_baslat(cihaz_st,&tx_kanal);
	kanal_baslat(cihaz_st,&rx_kanal);

	kanal_bilgi_yazdir(cihaz_st, &tx_kanal);
	kanal_bilgi_yazdir(cihaz_st, &rx_kanal);


	/*--------------------------------------------Ana Program Dongusu-----------------------------------------------------*/
	// To Do List
	// standartlarý gözden geçir
	// ana program döngüsünü https://github.com/Nuand/bladeRF/blob/master/host/libraries/libbladeRF/doc/examples/sync_rxtx.c yaz
	// test gerçekleþtir
	// programý arayüze entegre et



	/*--------------------------------------------Program Sonu-------------------------------------------------------------*/

	bladerf_free_device_list(cihaz_listesi_st);
	if (cihaz_st != NULL) {
		bladerf_close(cihaz_st);
	}
	printf("Program sonlandirildi.\n");

	return 0;
}


/*
int main() {
	int status;
	struct bladerf_devinfo* device_list = NULL;
	struct bladerf* device = NULL;
	int device_count = 0;

	// Cihaz listesini al
	device_count = bladerf_get_device_list(&device_list);
	if (device_count < 0) {
		fprintf(stderr, "Cihaz listesi alinamadi: %s\n", bladerf_strerror(device_count));
		return -1;
	}

	if (device_count == 0) {
		fprintf(stderr, "Cihaz bulunamadi.\n");
		return -1;
	}

	// Bulunan cihazlarý listele
	printf("Bulunan cihazlar:\n");
	for (int i = 0; i < device_count; i++) {
		printf("Cihaz %d: Seri No: %s\n", i, device_list[i].serial);
	}

	// Ýlk cihazý açmaya çalýþ
	printf("Ilk cihaz aciliyor...\n");
	status = bladerf_open_with_devinfo(&device, &device_list[0]);
	if (status != 0) {
		fprintf(stderr, "Cihaz acilamadi: %s\n", bladerf_strerror(status));
		bladerf_free_device_list(device_list);
		return -1;
	}

	printf("Cihaz basariyla acildi!\n");

	// Burada cihaz ile çalýþabilirsiniz...











	// Kaynaklarý temizle
	bladerf_close(device);
	bladerf_free_device_list(device_list);

	return 0;
}

burda cihaz ile çalýþabilirisiniz kýsmýna odaklanýcaz.*/





/*void repeater_example(struct bladerf* device) {

	int16_t tx_samples[TX_BUFFER_SIZE];
	int16_t rx_samples[RX_BUFFER_SIZE];


	CHECK_STATUS(
		bladerf_set_frequency(device, BLADERF_CHANNEL_RX(0), 2400000000),
		"RX frekansi ayarlanmadi"
	);

	// TX frekansini 2.4 GHz olarak ayarla


	// RX ornekleme oranini 1 MSPS olarak ayarla
	CHECK_STATUS(
		bladerf_set_sample_rate(device, BLADERF_CHANNEL_RX(0), 1000000, NULL),
		"RX ornekleme hizi ayarlanmadi"
	);


	// RX kazancini 40 dB olarak ayarla
	CHECK_STATUS(
		bladerf_set_gain(device, BLADERF_CHANNEL_RX(0), 40),
		"RX kazanci ayarlanmadi"
	);

	// TX kazancini 50 dB olarak ayarla


	// RX modulu etkinlestir
	CHECK_STATUS(
		bladerf_enable_module(device, BLADERF_CHANNEL_RX(0), true),
		"RX modulu etkinlestirilemedi"
	);



	// TX modulu etkinlestir
	CHECK_STATUS(
		bladerf_enable_module(device, BLADERF_CHANNEL_TX(0), true),
		"TX modulu etkinlestirilemedi"
	);

	// Full-duplex dongusu: Rx'ten veri al ve Tx'e gonder
	for (int i = 0; i < 10; i++) {
		// Rx'ten veri al
		CHECK_STATUS(
			bladerf_sync_rx(device, rx_samples, RX_BUFFER_SIZE, NULL, 5000),
			"RX islemi basarisiz"
		);

		// Alinan veriyi Tx'e gonder
		CHECK_STATUS(
			bladerf_sync_tx(device, rx_samples, RX_BUFFER_SIZE, NULL, 5000),
			"TX islemi basarisiz"
		);

		// Alinan ilk ornegi yazdir
		printf("RX verisi (ilk ornek) -> TX verisi: %d\n", rx_samples[0]);
	}

cleanup:
	// Programdan cikmadan once TX ve RX modullerini devre disi birak
	bladerf_enable_module(device, BLADERF_CHANNEL_TX(0), false);
	bladerf_enable_module(device, BLADERF_CHANNEL_RX(0), false);
}




DURUM_KONTROL(
	bladerf_set_sample_rate(cihaz_st, BLADERF_CHANNEL_TX(0), TX_SAMPLE_RATE, NULL),
	"TX ornekleme hizi ayarlanmadi ! "
);
DURUM_KONTROL(
	bladerf_set_frequency(cihaz_st, BLADERF_CHANNEL_TX(0), TX_FREQ),
	"TX frekansi ayarlanmadi ! "
);
DURUM_KONTROL(
	bladerf_set_gain(cihaz_st, BLADERF_CHANNEL_TX(0), TX_GAIN),
	"TX kazanci ayarlanmadi ! "
);
DURUM_KONTROL(
	bladerf_set_bandwidth(cihaz_st, BLADERF_CHANNEL_TX(0), TX_BANDWIDTH, 0),
	"TX kazanci ayarlanmadi ! "
);



DURUM_KONTROL(
	bladerf_set_sample_rate(cihaz_st, BLADERF_CHANNEL_RX(0), RX_SAMPLE_RATE, NULL),
	"TX ornekleme hizi ayarlanmadi ! "
);
DURUM_KONTROL(
	bladerf_set_frequency(cihaz_st, BLADERF_CHANNEL_RX(0), RX_FREQ),
	"TX frekansi ayarlanmadi ! "
);
DURUM_KONTROL(
	bladerf_set_gain(cihaz_st, BLADERF_CHANNEL_RX(0), RX_GAIN),
	"TX kazanci ayarlanmadi ! "
);
DURUM_KONTROL(
	bladerf_set_bandwidth(cihaz_st, BLADERF_CHANNEL_RX(0), RX_BANDWIDTH, 0),
	"TX kazanci ayarlanmadi ! "
);


*/