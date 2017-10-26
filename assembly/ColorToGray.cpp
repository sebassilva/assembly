// ColorToGray.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include <iostream>

cv::Mat img_original;
cv::Mat img_grayscale;
cv::Mat img_grayscale_cpp;
cv::Mat img_grayscale_asm;

void callback_click(int event, int x, int y, int flags, void* userdata)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		std::cout << std::endl;
		std::cout << "Image clicked in (" << x << ", " << y << ")" << std::endl;
		std::cout << "Index of pixel: "<< (y * img_original.cols + x)<< std::endl;
		int idx_b = (y*img_original.step + x*img_original.channels());
		int idx_g = idx_b + 1;
		int idx_r = idx_b + 2;
		std::cout << "Indices of BGR for this pixel: " << idx_b << ", " << idx_g << ", " << idx_r << std::endl;
		std::cout << "BGR values for this pixel: ";
		std::cout << (int)img_original.data[idx_b] << ", " << (int)img_original.data[idx_g] << ", " << (int)img_original.data[idx_r] << std::endl;
	} 
}

int colorToGray()
{
	img_original  = cv::imread("Lena.jpg", CV_LOAD_IMAGE_COLOR);
	img_grayscale = cv::Mat(img_original.rows, img_original.cols, CV_8UC1);

	img_grayscale_cpp = cv::Mat(img_original.rows, img_original.cols, CV_8UC1);
	img_grayscale_asm = cv::Mat(img_original.rows, img_original.cols, CV_8UC1);



	cv::namedWindow("Original Image");
	cv::imshow("Original Image", img_original);
	cv::setMouseCallback("Original Image", callback_click, NULL);

	std::cout << "DATA FOR THE ORIGINAL IMAGE:" << std::endl;
	std::cout << "Image width= " << img_original.cols << std::endl;
	std::cout << "Image height= " << img_original.rows << std::endl;
	std::cout << "Number of channels= " << img_original.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_original.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_original.step << std::endl;

	std::cout << "DATA FOR THE GRAYSCALE IMAGE:" << std::endl;
	std::cout << "Image width= " << img_grayscale.cols << std::endl;
	std::cout << "Image height= " << img_grayscale.rows << std::endl;
	std::cout << "Number of channels= " << img_grayscale.channels() << std::endl;
	std::cout << "Element size in bytes= " << img_grayscale.elemSize() << std::endl;
	std::cout << "Row size in bytes= " << img_grayscale.step << std::endl;

	long init_count;
	long end_count;
	
	
	init_count = cv::getTickCount();
	cv::cvtColor(img_original, img_grayscale, cv::COLOR_BGR2GRAY);
	end_count = cv::getTickCount();
	std::cout << "Elapsed time [s] with OpenCV: " << (end_count - init_count)/cv::getTickFrequency() << std::endl;


	//Pasando a escala de grises en C
	init_count = cv::getTickCount();
	for (int i = 0; i < (img_original.rows*img_original.cols); i++) {
		int b = img_original.data[3 * i];
		int g = img_original.data[3 * i + 1];
		int r = img_original.data[3 * i + 2];
		img_grayscale_cpp.data[i] = (b + g + r) / 3;
	}
	end_count = cv::getTickCount();
	std::cout << "Elapsed time [s] with C++: " << (end_count - init_count) / cv::getTickFrequency() << std::endl;



	//Utilizando ensablador
	init_count = cv::getTickCount();

	__asm {
		//mov edi, img_original.data				//Cargamos la información de la imagen original
		//mov esi, img_grayscale_asm.data			//Obtenemos la direccion de memoria donde escribiremos
		//mov ecx, img_original.cols				//Obtenemos las columnas de la imagen original
		//imul ecx, img_original.rows				//Obtenemos las filas
		//imul ecx, 3								//Multiplicamos por 3 porque tenemos 3 canales
		//add ecx, edi							//Obtenemos la última direccion de memoria		
		//mov bl, 3								//Movemos el 4 porque este será nuestro divisor
		//xor edx, edx							//Iniciamos edx en 0
	
		//for_loop: 
		//	xor eax, eax						//inicializamos eax en 0	
		//	mov al, [edi]						//
		//	mov dl, [edi+1]
		//	add ax, dx
		//	mov dl, [edi+2]
		//	add ax, dx

		//	div bl	;;; Dividiendo en ax, divisor en dl
		//	mov[esi], al
		//	add edi, 3
		//	inc esi
		//	cmp edi, ecx
		//	jl for_loop

	}
	end_count = cv::getTickCount();
	std::cout << "Elapsed time [s] with asm: " << (end_count - init_count) / cv::getTickFrequency() << std::endl;

	//Mostramos todas las imágeness
	cv::imshow("Grayscale Image", img_grayscale);
	cv::imshow("Grayscale Image with c++", img_grayscale_cpp);
	cv::imshow("Grayscale Image with asm", img_grayscale_asm);

	while (cv::waitKey(15) != 27)
	{

	}
    return 0;
}


void gaussianBlur() {
	cv::Mat img_original =  cv::imread("Lena.jpg");
	cv::cvtColor(img_original, img_original, cv::COLOR_BGR2GRAY);
	cv::Mat img_blur = cv::Mat(img_original.rows, img_original.cols, CV_8UC1);

	int kernel_size = 4;
	int kernel_length = (4 * kernel_size * kernel_size + 4 * kernel_size + 1);
	float* kernel = (float*)malloc(kernel_length * sizeof(float));

	for (int i = 0; i < kernel_size ; i++) {
		kernel[i] = 1.0 / kernel_length; 
	}
	std::cout << "Kernel size = " << kernel_size << std::endl;
	std::cout << "ROWS: " << img_original.rows;
	std::cout << "COLS: " << img_original.cols;

	for (int i = kernel_size; i < (img_original.rows - kernel_size); i++) {
		for (int j = kernel_size; j < (img_original.cols - kernel_size); j++) {
			float sum = 0;

			for (int k1 = -kernel_size; k1 <= kernel_size; k1++) {
				for (int k2 = -kernel_size; k2 <= kernel_size; k2++) {
					
					int pix_x = i + k1;
					int pix_y = j + k2;
					int kernel_x = k1 + kernel_size;
					int kernel_y = k2 + kernel_size;


					sum += img_original.data[pix_x * img_original.cols + pix_y] * kernel[kernel_x * (2 * kernel_size + 1) + kernel_y];
				}
			}
			img_blur.data[(i * img_original.cols) + j] = (int)sum;
		
		}
		
	}

	cv::imshow("Grayscale Image original", img_original);
	cv::imshow("Grayscale Image with asm", img_blur);

	//free(kernel);
}



void asmGaussianBlur() {
	int i, j, k1, k2, pixel;
	float sum;
	cv::Mat img_original = cv::imread("Lena.jpg");
	cv::cvtColor(img_original, img_original, cv::COLOR_BGR2GRAY);
	cv::Mat img_blur = cv::Mat(img_original.rows, img_original.cols, CV_8UC1);

	int kernel_size = 4;
	int kernel_length = (4 * kernel_size * kernel_size + 4 * kernel_size + 1);
	float* kernel = (float*)malloc(kernel_length * sizeof(float));

	for (int i = 0; i < kernel_size; i++) {
		kernel[i] = 1.0 / kernel_length;
	}


	__asm {
	


	mov eax, kernel_size								//Inicializamos el primer for loop en el pixel inicial, desplazado kernel_size veces
	mov i, eax

	for_loop_cols:										//Recorremos columnas con j

		
		mov edx, kernel_size						//Contador para cols
		mov j, edx
			for_loop_rows:
				mov sum, 0
				mov eax, img_original.cols					
				sub eax, kernel_size						//Hasta de for loop

				//Iteramos sobre los pixeles del kernel

				mov ebx, kernel_size
				neg ebx
				mov k1, ebx


				for_loop_kernel_rows:
					
					//Iteramos sobre las columnas del kernel 
					mov ecx, kernel_size
					neg ecx
					mov k2, ecx
					for_loop_kernel_cols: 
						
						// Aquí hacemos el producto de la matriz gaussiana
						
						call get_indexes

						inc k2
						mov ebx, k2
						cmp ebx, kernel_size
						jl for_loop_kernel_cols


					inc k1
					mov ebx, k1
					cmp ebx, kernel_size
					jl for_loop_kernel_rows

			//Escribir en la nueva imagen y convertir el float a int
			// i * img_original.cols) + j


			mov ebx, img_original.cols
			imul ebx, i
			add ebx, j
			
			mov ecx, img_blur.data
			add ecx, ebx					//Direccion de memoria actual de la nueva imagen

			mov ebx, sum
			mov edx, 0
			mov dl, bl
	
			mov  [ecx], dl //PENDIENTE DE HACER  ENTERO CAST


			mov eax, img_original.cols							//Obtenemos renglones
			sub eax, kernel_size							//Obtenemos columnas
			inc j
			cmp j, eax
			jl for_loop_rows

			
			
		inc i
		mov eax, img_original.rows							//Obtenemos renglones
		sub eax, kernel_size							//Obtenemos hasta donde debe llegar el primer for loop
		cmp i, eax
		jl for_loop_rows
	
	



	







		get_indexes: 

			//original_idx = img_original.data[(i + k1) * img_original.cols + (j + k2)]
			//kernek_idx = kernel[k1 + kernel_size * (2 * kernel_size + 1 ) + (k2 + kernel_size)]

			//index_original = (i + k1) * img_original.cols + (j + k2)
			//index_kernel = (k1 + kernel_size) * (2 * kernel_size + 1) + (k2 + kernel_size)

			mov edi, img_original.data

			//img_original.cols	
			////esi original index
			//edi kernel index
			//edx disponible
			mov esi, i
			add esi, k1     // ex = k1
			imul esi, img_original.cols
			add esi, j
			add esi, k2
		
			add edi, esi				//Esta es la direcci{on de memoria actual para la imagen
			mov edx, [edi]
			mov ebx, 0					//Limpiamos la variable
			mov bl, dl
			mov pixel, ebx
			fild pixel					//Guardamos el valor en st0


			//Para el kernel
			mov edi, 2
			imul edi, kernel_size
			inc edi					//2 * kernel_size + 1

			mov esi, k1
			add esi, kernel_size
			imul edi, esi				//  (2 * kernel_size + 1 ) * (k2 + kernel_size)
			add edi, k2
			add edi, kernel_size

			mov esi, [kernel]
			add edi, esi				//Dirección de memoria del kernel
		
			mov edx, [edi]
			mov ebx, 0
			mov bl, dl
			mov pixel, ebx
			fild pixel					//Guardamos el valor en st


			fmul st, st(1)
			fld sum
			fadd st, st(1)
			fst sum
				//liberamos todos los registros
			ffree st(0)
			ffree st(1)
			ffree st(2)
			ffree st(3)


		
			ret

		






	
	
	
	}

	cv::imshow("Grayscale Image with asm", img_blur);
	
	
	

}

int main() {
	//colorToGray();
	///gaussianBlur();
	asmGaussianBlur();

	while (cv::waitKey(15) != 27)
	{

	}
	return 0;
}