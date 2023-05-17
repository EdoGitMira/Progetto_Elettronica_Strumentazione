/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 *
 *PROGETTO RADAR INSEGUITORE DI UN OGGETTO
 *
 *UNIVERSITA DEGLI STUDI DI BRESCIA
 *
 *A.A.:2022/23
 *
 *Version: 2.1 sensore verticale
 *
 *STUDENTI: - FARES PETER WILLIAM GUIRGUIS -> p.fares001@studenti.unibs.it
 *			- MIRANDOLA EDOARDO -> e.mirandola@studenti.unibs.it
 *          - PENNACCHIO MANUELE -> m.pennacchio006@studenti.unibs.it
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define DISmax 300 // range massimo in mm di inseguimento dell'oggetto
#define DISmin 100 // range minimo in mm di inseguimento dell'oggetto
#define ANG 250    // offset per la conversione da angolo a valore di PWM
#define INCmax 25  // incremento angolo max se non trovo l'oggetto
#define INCmin 15  // incrementto standar dell'angolo per trovare l'oggetto
#define CONVAngolo 5.55555556 //varibile di conversione da angolo a valore di PWM 1000/180
#define CONVDist 0.17// varibile di conversione da tempo a mm 340/2000;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);

/* USER CODE BEGIN PFP */

// definizione prototipo funzione delay di microsecondi non accurato
void delayMicros(int micros);
// definizione prototipo funzione per lettura in un determinato angolo tra 0°-180°
int letturaAngolo(float angolo);

float letturaAngoloDist(float angolo);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	//definizione delle varibili locali

	float pos = 0.0;    //posizionamento del sensore nel centro dell'oggetto
	float pos1 = 0.0;   //posizionamento del sensore a dx dell'oggetto
	float pos2 = 0.0;   //posizionamento del sensore a sx dell'oggetto

	int ampOgg = 10; //valore di ampiezza in lettura per considerare un oggetto valido per l'inseguimento
	int check = 2;      //variabile appoggio utilizza per salvare l'indice del primo valore
	int count = 0;      // variabile di conteggio per intervallo di presenza dell'oggetto
	int incAng = 10;    //valore di incremento dell'angolo per l'inseguimento dell'oggetto
	int indice2 = 0;    //indice primo valore differente da 0 nell'array dt
	int indicescan = 0; //indice per la scansione dei dati filtrati e come end point dei dati
	int let = 0;        //variabile per la presenza o meno dell'oggetto nel centro
	int let1 = 0;       //variabile per la presenza o meno dell'oggetto a dx
	int let2 = 0;       //variabile per la presenza o meno dell'oggetto a sx
	int stato = 2.;     //variabile per la gestione dello stato corente del radar

	long int lettura = 0; // valore della distanza letta in mm dell'oggetto
	volatile int dt[180]; // array contenete i dati della scansione


	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	/* USER CODE END 2 */

	/* Infinite loop */

	/* USER CODE BEGIN WHILE */
	TIM3->CCR3= 750;//posizionamento del sensore a metà del range di scansione del servomotore
	delayMicros(10000);//delay di attesa per poter vedere dove si è posizionato
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		//SCANSIONE DI 180° NEI DUE SENSI PER TROVARE L'OGGETTO
		if (stato == 2){
			//SCANSIONE IN SENSO ORARIO DI 180°
			TIM3->CCR3= 250;//posizionamento del sensore a 0°
			delayMicros(10000);//delay di attesa per posizionamento
			for(int indice =0 ; indice<181; indice++ ){
				lettura = letturaAngoloDist(indice); //richiamo funzione di lettura distanza
				if(lettura > DISmin && lettura < DISmax){//metto 0 se il valore letto è fuori dal range
					dt[indice] = lettura;
				}else{
					dt[indice] = 0;
				}
			}
		}else if(stato==3) {
			//SCANSIONE IN SENSO ANTIORARIO DI 180°
			TIM3->CCR3= 1250;//posizionamento del sensore a 180°
			delayMicros(10000);//delay di attesa per posizionamento
			for(int indice =180 ; indice>0; indice-- ){
				lettura = letturaAngoloDist(indice);//richiamo funzione di lettura distanza
				if(lettura > DISmin && lettura < DISmax){//metto 0 se il valore letto è fuori dal range
					dt[indice] = lettura;
				}else{
					dt[indice] = 0;
				}
			}
		}

		/*filtro sui valori letti su 5 campioni che possono darivare da valori di lettura
		* per confronto sui limiti del range si pongono uguali a zero
		*/
		for(int j=2; j<181; j = j+2){
			if(dt[j-2] == dt[j+2] && dt[j-2] == 0){
				dt[j-1] = 0;
				dt[j] = 0;
				dt[j+1] = 0;
			}
		}

		/* analisi dei dati filtrati e si procede con la indentificazione dell'oggetto
		*  si salva l'indice del primo valore delle letture != 0
		*  e si conta per quante volte è presnte un valore !=0 se > di 15 ho trovato l'oggetto
		*  se no continuo con l'analisi dei dati se non verifico le condizioni setto una variabile
		*  in modo da riscansionare l'area di misura
		*/

		//riniazzilizzazione variabili del filtro
		indice2 = 0;
		count = 0;
		check = 2;
		for(indicescan=0; indicescan<181; indicescan++){
			if(dt[indicescan] >1){
				if(check > 1){
					count++;//incremento per valore !=0
					indice2 = indicescan;//salvo indice val !=0
					check = 1;
				}else{
					count++;//incremento per valore !=0
				}
			}else{
				if(count>ampOgg){//se ho un oggetto interrompo il ciclo
					stato = 1;// ho trovato un oggetto che rispetta le specifiche allora interrompo il ciclo e vado nel while successivo per l'inseguimento
					break;
				}else{
					indice2 = 0;
					count = 0;//azzeramento del conteggio
					check = 2;// reset della varibile
				}
			}
		}
		//condizione per porre stato = 1 e inseguire  nel caso l'oggetto sia al limite di 180°
		if(indicescan == 181 && count >ampOgg){
			stato = 1;
		}

		//check di stato per decidere cosa devo eseguire
		if(stato == 2){//riscansiono in senso antiorario
			stato = 3;
		}else if(stato ==3){//riscansiono in senso orario
			stato = 2;
		}else if(stato ==1){//oggetto trovato calcolo la posizione
			pos = (((float)(indice2+indicescan-1))/2.0); // posizione centrale dell'oggetto
		}

		while(stato<2){//seguo l'oggetto

			//calcolo due posizioni vicino a pos per fare l'insegumento e leggo la presenza o meno dell'oggetto
			pos1 = pos -incAng;
			pos2 = pos +incAng;
			let1= letturaAngolo(pos1);
			let = letturaAngolo(pos);
			let2= letturaAngolo(pos2);

			/* di seguito in base alla risposta del sensore delle letture si decide
			 * la posizone centrale del sensore per poter fare il tracking dell'oggetto
			*/
			if(let <1 && let1<1 && let2<1){
				if(incAng>10){
					// nel caso di perdità dell'oggetto rieseguo una scansione partendo dal valore estremo più vicino a dove sono posizionato
					if(pos>90){ //se sono in posiizone >90°
						stato = 3;//parto da 180°
					}else{
						stato = 2;//parto da 0°
					}
					incAng = INCmin;
				}else{
					incAng = INCmax;
					//controllo per i limiti del range di applicazione tra 0°-180°
					//per l'iterazione successiva con incAng
					if(pos>(180-incAng)){
						pos = (180-incAng);
					}else if(pos<incAng){
						pos = incAng;
					}
				}

			}else{
				if(incAng>INCmin){
					incAng = INCmin;
				}

				if(let >1 && let1>1 && let2<1){
					pos = (pos +pos1)/2; // mi posiziono a metà del range
				}else if(let >1 && let2>1 && let1<1){
					pos = (pos +pos2)/2; // mi posiziono a metà del range
				}else if(let1>1 && let<1 && let2<1){
					pos = pos1 - incAng; // mi posiziono in manierà tale di essere più a sx del valore letto dove probabilmente c'è l'oggetto
				}else if(let2>1 && let<1 && let1<1){
					pos = pos2 + incAng; // mi posiziono in manierà tale di essere più a dx del valore letto dove probabilmente c'è l'oggetto
				}

				//controllo per i limiti del range di applicazione tra 0°-180°
				if(pos>(180-incAng)){
					pos = (180-incAng);
				}else if(pos<incAng){
					pos = incAng;
				}
			}
		}
	}


	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 84;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 4294967295;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 168;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 10000;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 1000;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */
	HAL_TIM_MspPostInit(&htim3);

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_9, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LD2_Pin PA9 */
	GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PC7 */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
 * Funzione per creare un delay approssimativo in us
 */
void delayMicros(int micros){
	int i=0, j=0;
	for(i=0; i<micros; i++){
		for(j=0; j<7; j++){
			;
		}
	}
}

/**
 * Implemnetazione funzione per la lettutura in un determinanto angolo
 * e restituisce se ce o no l'oggetto nel range scelto
 */

int letturaAngolo(float angolo){

	TIM3->CCR3= ANG+angolo*CONVAngolo;
	delayMicros(80000);
	// genero un pulse su Trig
	GPIOA->BSRR=1<<9;
	delayMicros(10);
	GPIOA->BSRR=1<<(9+16);
	// azzero il valore del timer
	TIM2->CNT=0;
	// attendo echo up
	while((GPIOC->IDR & 1<<7)<1);
	// inizio la misura del tempo
	HAL_TIM_Base_Start(&htim2);
	// polling di Echo up
	while((GPIOC->IDR & 1<<7)>0) {
		;
	}
	// fermo il timer quando Echo torna down
	HAL_TIM_Base_Stop(&htim2);
	//distanza misurata in mm
	float lettura = TIM2->CNT*CONVDist;
	if(lettura > DISmin && lettura < DISmax){
		return 2; // se l'oggetto è nel range
	}else{
		return 0; // se l'oggetto è fuori dal range
	}
}
/**
 * Implemnetazione funzione per la lettutura in un determinanto angolo
 * e restituisce la distanza in mm
 */

float letturaAngoloDist(float angolo){

	TIM3->CCR3= ANG+angolo*CONVAngolo;
	delayMicros(10000);
	// genero un pulse su Trig
	GPIOA->BSRR=1<<9;
	delayMicros(10);
	GPIOA->BSRR=1<<(9+16);
	// azzero il valore del timer
	TIM2->CNT=0;
	// attendo echo up
	while((GPIOC->IDR & 1<<7)<1);
	// inizio la misura del tempo
	HAL_TIM_Base_Start(&htim2);
	// polling di Echo up
	while((GPIOC->IDR & 1<<7)>0) {
		;
	}
	// fermo il timer quando Echo torna down
	HAL_TIM_Base_Stop(&htim2);
	//distanza misurata in mm
	return TIM2->CNT*CONVDist;
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
