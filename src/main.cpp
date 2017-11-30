#include "CImg.h"
#include <iostream>

using namespace cimg_library;

int main(int argc, char const ** argv) {
	

	if ( argc != 4) {
		std::cout << "USAGE : ./main image_F image_G nbre_iteration" << std::endl;
		return 0;
	}

	float voxels_size[3];
	CImg<> img_F, img_G;
	img_F.load_analyze ( argv[1], voxels_size );
	img_G.load_analyze ( argv[2], voxels_size );
	int nbr_iteration = atoi ( argv[3] );
	int i = 0;

	CImgList<> list = img_F.get_gradient ("xyz");
	CImgList<> field_vector = list;

	int height = img_F.height ();
	int width = img_F.width ();
	int depth = img_F.depth ();

	CImg<> img_GPrime = img_G;

	while ( i < nbr_iteration) {
		// vecteur de déplacement
		for ( int j = 0; j < height; j++ ) {
			for ( int i = 0; i < width; i++ ) {
				for ( int k = 0; k < depth; k++ ) {
					float f = img_F (i, j, k);
					float g = img_GPrime (i, j, k);
					float vector [3] = {0.f, 0.f, 0.f};
					float x = list[0] (i, j, k);
					float y = list[1] (i, j, k);
					float z = list[2] (i, j, k);

					if ( f != g && (x != 0 || y != 0 || z != 0) ) {
						float norme = pow ( x, 2 ) + pow ( y, 2 ) + pow ( z, 2 );
						// norme = sqrt (norme);
						float g_f = g - f;
						vector[0] = ( g_f * x ) / norme;
						vector[1] = ( g_f * y ) / norme;
						vector[2] = ( g_f * z ) / norme;
					}

					field_vector[0](i, j, k) = vector[0];
					field_vector[1](i, j, k) = vector[1];
					field_vector[2](i, j, k) = vector[2];
				}
			}
		}
		//*********************************  end


		// déformation de G
		field_vector[0] = field_vector[0].blur (5, 5, 5, true, true);
		for ( int j = 0; j < height; j++ ) {
			for ( int i = 0; i < width; i++ ) {
				for ( int k = 0; k < depth; k++ ) {
					img_GPrime (i, j, k) = field_vector[0](i, j, k);
				}
			}
		}

		i++;
	}

	CImgDisplay img_FDisp ( img_F, "image Fixe");
	CImgDisplay img_GDisp ( img_G, "image Flottante");
	CImgDisplay img_GDispPrime ( img_GPrime, "image Flottante déformée");

	while (!img_FDisp.is_closed() && !img_GDisp.is_closed() && !img_GDispPrime.is_closed()) {
		img_GDisp.wait ();
	}

	return 0;
}