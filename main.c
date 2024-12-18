#include "libbladeRF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <assert.h>
#include"bladeRF2.h" // bias tee ayari icin include edildi



#define TX_FREQ			(uint64_t)(1575420000)	//Hz
#define TX_SAMPLE_RATE			  (12000000)		//Hz
#define TX_BANDWIDTH		      (50000000)		//Hz
#define TX_GAIN					  (45)			//Db45
#define TX_TIMEOUT				  (600)			//ms


#define RX_FREQ		    (uint64_t)(1575420000)	//Hz
#define RX_SAMPLE_RATE			  (12000000)		//Hz
#define RX_BANDWIDTH			  (50000000)		//Hz
#define RX_GAIN					  (45)			//Db40
#define RX_TIMEOUT			      (600)			//ms

#define ORNEK_ALMA_GONDERME_SAYISI (100000)



#define DURUM_KONTROL(fonksiyon, mesaj_u8a) \
    do { \
        uint8_t durum_u8 = (fonksiyon); \
        if (durum_u8 != 0) { \
            fprintf(stderr, "Hata: %s -> %s\n", (mesaj_u8a), bladerf_strerror(durum_u8)); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)



typedef enum {
	DURUM_BEKLIYOR,
	DURUM_TAMAMLANDI
} islem_durumu_t;

typedef enum {
	VERI_YOK,
	VERI_VAR
} veri_durumu_t;


typedef enum
{
	TX1 = BLADERF_CHANNEL_TX(0),
	TX2 = BLADERF_CHANNEL_TX(1),
	RX1 = BLADERF_CHANNEL_RX(0),
	RX2 = BLADERF_CHANNEL_RX(1),
}kanal_port_t;
typedef enum 
{
	BIAS_TEE_KAPALI = 0,
	BIAS_TEE_AKTIF = 1,
	
} bias_tee_t;

typedef struct
{
	bladerf_channel         channel_i32;
	bladerf_sample_rate		sample_rate_u32;
	bladerf_frequency		frequency_u64;
	bladerf_gain		    gain_i32;
	bladerf_bandwidth		bandwidth_u32;
	bias_tee_t				bias_tee_et;
}kanal_t;




void kanal_baslat(struct bladerf* cihaz_st, kanal_t* kanal_st)
{
	printf("Kanal (%s) parametreleri ayarlaniyor.\n", (kanal_st->channel_i32 == BLADERF_CHANNEL_TX(1)) ? "TX" : "RX");

	DURUM_KONTROL(
		bladerf_set_sample_rate(cihaz_st, kanal_st->channel_i32, kanal_st->sample_rate_u32, NULL),
		"Kanal ornekleme hizi ayarlanamadi ! "
	);

	DURUM_KONTROL(
		bladerf_set_frequency(cihaz_st, kanal_st->channel_i32, kanal_st->frequency_u64),
		"Kanal frekansi ayarlanamadi ! "
	);

	DURUM_KONTROL(
		bladerf_set_gain(cihaz_st, kanal_st->channel_i32, kanal_st->gain_i32),
		"Kanal kazanci ayarlanamadi ! "
	);

	DURUM_KONTROL(
		bladerf_set_bandwidth(cihaz_st, kanal_st->channel_i32, kanal_st->bandwidth_u32, 0),
		"Kanal bant genisligi ayarlanamadi ! "
	);

	
	// todo bias teee
	DURUM_KONTROL(
		bladerf_enable_module(cihaz_st, kanal_st->channel_i32, true),
		"Kanal aktiflestirilemedi ! "
	);

	printf("Kanal (%s) basariyla baslatildi.\n", (kanal_st->channel_i32 == BLADERF_CHANNEL_TX(1)) ? "TX" : "RX");

}

void kanal_bilgi_yazdir(struct bladerf* cihaz_st, kanal_t* kanal_st)
{
	bladerf_sample_rate sample_rate;
	bladerf_frequency	frequency;
	bladerf_gain		gain;
	bladerf_bandwidth	bandwidth;
	bias_tee_t			bias_tee_et;

	DURUM_KONTROL(
		bladerf_get_sample_rate(cihaz_st, kanal_st->channel_i32, &sample_rate),
		"Ornekleme hizi alinamadi"
	);
	printf("Kanal: %s - Ornekleme Hizi: %d Hz\n", (kanal_st->channel_i32 == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX", sample_rate);


	DURUM_KONTROL(
		bladerf_get_frequency(cihaz_st, kanal_st->channel_i32, &frequency),
		"Frekans alinamadi"
	);
	printf("Kanal: %s - Frekans: %ld Hz\n", (kanal_st->channel_i32 == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX", frequency);

	DURUM_KONTROL(
		bladerf_get_gain(cihaz_st, kanal_st->channel_i32, &gain),
		"Kazanc alinamadi"
	);
	printf("Kanal: %s - Kazanc: %d dB\n", (kanal_st->channel_i32 == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX", gain);


	DURUM_KONTROL(
		bladerf_get_bandwidth(cihaz_st, kanal_st->channel_i32, &bandwidth),
		"Bant Genisligi alinamadi"
	);
	printf("Kanal: %s - Bant Genisligi: %d Hz\n", (kanal_st->channel_i32 == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX", bandwidth);

	DURUM_KONTROL(
		bladerf_get_bias_tee(cihaz_st, kanal_st->channel_i32, &bias_tee_et),
		"Bias Tee alinamadi"
	);
	printf("Kanal: %s - Bias Tee Durumu: %s\n",(kanal_st->channel_i32 == BLADERF_CHANNEL_TX(0)) ? "TX" : "RX", (bias_tee_et == BIAS_TEE_AKTIF) ? "Aktif" : "Kapali"); // hatali 
}



/* Just retransmit the received samples */
/* From example*/
bool do_work(int16_t* rx, unsigned int rx_len, bool* have_tx_data, int16_t* tx, unsigned int tx_len)
{
	static unsigned int call_no = 0;
	assert(tx_len == rx_len);
	memcpy(tx, rx, rx_len * 2 * sizeof(int16_t));
	*have_tx_data = true;

	return (++call_no >= ORNEK_ALMA_GONDERME_SAYISI);
}

// todolarý hallet

int main() {

	struct bladerf_devinfo* cihaz_listesi_sta	= NULL;
	struct bladerf*			cihaz_st			= NULL;
	int16_t					cihaz_sayisi_i16	= 0;
	kanal_t tx_kanal_st							= { 0 };
	kanal_t rx_kanal_st							= { 0 };
	
	/*-------------------------------------------Bagli Cihazlari Kontrol Et--------------------------------------------*/
	
	cihaz_sayisi_i16 = bladerf_get_device_list(&cihaz_listesi_sta);

	if (cihaz_sayisi_i16 < 0)
	{
		fprintf(stderr, "Cihaz listesi alinamadi: %s\n", bladerf_strerror(cihaz_sayisi_i16));
		return -1;
	}

	printf("Bulunan cihazlar:\n");
	for (int i = 0; i < cihaz_sayisi_i16; i++) 
	{
		printf("Cihaz %d: Cihaz Model: %s, Seri No: %s\n", i, cihaz_listesi_sta[i].product, cihaz_listesi_sta[i].serial);
	
	}

	/*--------------------------------------------Cihazi Ac-----------------------------------------------------------*/
	
	DURUM_KONTROL(
		bladerf_open_with_devinfo(&cihaz_st, &cihaz_listesi_sta[0]),
		"Cihaz acilamadi!!"
	);
	printf("Cihaz basariyla acildi!\n");


	/*--------------------------------------------Cihaz Kanal Parametrelerini Kur --------------------------------------------*/

	tx_kanal_st.channel_i32			= TX2;
	tx_kanal_st.sample_rate_u32		= TX_SAMPLE_RATE;
	tx_kanal_st.frequency_u64		= TX_FREQ;
	tx_kanal_st.gain_i32			= TX_GAIN;
	tx_kanal_st.bandwidth_u32		= TX_BANDWIDTH;
	tx_kanal_st.bias_tee_et			= BIAS_TEE_KAPALI;

	rx_kanal_st.channel_i32			= RX1;
	rx_kanal_st.sample_rate_u32		= RX_SAMPLE_RATE;
	rx_kanal_st.frequency_u64		= RX_FREQ;
	rx_kanal_st.gain_i32			= RX_GAIN;
	rx_kanal_st.bandwidth_u32		= RX_BANDWIDTH;
	rx_kanal_st.bias_tee_et			= BIAS_TEE_AKTIF;

	/*DURUM_KONTROL(
		bladerf_set_bias_tee(cihaz_st, BLADERF_CHANNEL_RX(0), 1),
		"Kanal Bias Tee  ayarlanamadi ! "
	);*/
	bladerf_set_gain_mode(cihaz_st, BLADERF_CHANNEL_RX(0), BLADERF_GAIN_MGC);
	
	uint8_t a;
	bladerf_set_bias_tee(cihaz_st, BLADERF_CHANNEL_RX(0), 1);
	bladerf_get_bias_tee(cihaz_st, BLADERF_CHANNEL_RX(0), &a);
	printf("%d \n ", a);



	kanal_baslat(cihaz_st,&tx_kanal_st);
	kanal_bilgi_yazdir(cihaz_st, &tx_kanal_st);


	kanal_baslat(cihaz_st,&rx_kanal_st);
	kanal_bilgi_yazdir(cihaz_st, &rx_kanal_st);

	/*--------------------------------------------Ana Program Ayarlari (Sync)-----------------------------------------------------*/
	int8_t durum_i8;
	islem_durumu_t tamam			= DURUM_BEKLIYOR;
	veri_durumu_t  tx_verisi_var	= VERI_YOK;


	/*--------TX RX ornekleri tampon ayarlari-----*/
	int16_t* rx_ornekleri_i16a			= NULL;
	int16_t* tx_ornekleri_i16a			= NULL;
	const uint16_t ornek_uzunlugu_u16	= 32768; //TODO // define 


	rx_ornekleri_i16a = malloc(ornek_uzunlugu_u16 * 2 * 1 * sizeof(int16_t));

	if (rx_ornekleri_i16a == NULL) 
	{
		perror("malloc");
		return BLADERF_ERR_MEM;
	}


	tx_ornekleri_i16a = malloc(ornek_uzunlugu_u16 * 2 * 1 * sizeof(int16_t));

	if (tx_ornekleri_i16a == NULL)
	{
		perror("malloc");
		free(rx_ornekleri_i16a);
		return BLADERF_ERR_MEM;
	}


	/*-------Senkron parametre Ayarlamalari------*/

	uint8_t		tampon_sayisi_u8		= 32;	//TODO //define
	uint32_t	tampon_boyutu_u32		= 32768; /* Must be a multiple of 1024 */
	uint8_t     veri_transfer_sayisi_u8 = 16;
	uint16_t    zaman_asimi_16t			= 1000;

	DURUM_KONTROL(
		bladerf_sync_config(cihaz_st, BLADERF_TX_X1, BLADERF_FORMAT_SC16_Q11,
			tampon_sayisi_u8, tampon_boyutu_u32, veri_transfer_sayisi_u8,
			zaman_asimi_16t),
		"Senkron TX ayari basarisiz !"
	);

	DURUM_KONTROL(
		bladerf_sync_config(cihaz_st, BLADERF_RX_X1, BLADERF_FORMAT_SC16_Q11,
			tampon_sayisi_u8, tampon_boyutu_u32, veri_transfer_sayisi_u8,
			zaman_asimi_16t),
		"Senkron RX ayari basarisiz !"
	);

	durum_i8 = 0;


	/*--------------------------------------------Ana Program Dongusu-----------------------------------------------------*/

	printf("Islem basladi !");
	while (durum_i8 == 0 && !tamam)
	{
		// ornek al
		durum_i8 = bladerf_sync_rx(cihaz_st, rx_ornekleri_i16a, ornek_uzunlugu_u16, NULL, RX_TIMEOUT);
		if (durum_i8 ==0)
		{
			tamam = do_work(rx_ornekleri_i16a, ornek_uzunlugu_u16, &tx_verisi_var, tx_ornekleri_i16a, ornek_uzunlugu_u16);
			// ornek gonder
			if (!tamam && tx_verisi_var)
			{
		
				durum_i8 = bladerf_sync_tx(cihaz_st, tx_ornekleri_i16a, ornek_uzunlugu_u16, NULL, TX_TIMEOUT);
				if (durum_i8 != 0)
				{
					printf("TX ornegi gonderilemedi ! ");
				}
			}
		}
		else
		{
			printf("RX ornegi alinamadi ! ");
		}


	}

	/*--------------------------------------------Program Sonu-------------------------------------------------------------*/

	bladerf_free_device_list(cihaz_listesi_sta);
	if (cihaz_st != NULL) {
		bladerf_close(cihaz_st);
	}
	// TODO
	// kanal kapat fonksiyonu // bladerf_enable_module(dev, BLADERF_RX, false);
	printf("Program sonlandirildi.\n");

	// TODO : kodu moduler yapabilirsin
	return 0;
}

// bandhwidht max 56Mhz
// glonas dahil için gerekli +-35MHZ : test edilecek
