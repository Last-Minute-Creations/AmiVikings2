#include "bitmap.h"
#include <fmt/format.h>
#include <cmath>
#include <cstdint>
#include <chrono>

static int64_t calcError(const tChunkyBitmap &Img, const std::vector<tRgb> &vColors)
{
	int64_t Error = 0;
	for(const auto &Pixel: Img.m_vData) {
		if(Pixel.ubR == 0xFF && Pixel.ubG == 0x00 && Pixel.ubB == 0xFF) {
			continue;
		}
		int MinError = 0xFFFF * 3;
		for(const auto &Color: vColors) {
			auto DeltaR = ((int)Pixel.ubR - (int)Color.ubR);
			auto DeltaG = ((int)Pixel.ubG - (int)Color.ubG);
			auto DeltaB = ((int)Pixel.ubB - (int)Color.ubB);
			auto ColorError = DeltaR*DeltaR + DeltaG*DeltaG + DeltaB*DeltaB;
			if(ColorError < MinError) {
				MinError = ColorError;
			}
		}
		Error += MinError;
	}
	return Error;
}

static void replaceColors(tChunkyBitmap &Img, const std::vector<tRgb> &vColors) {
	for(auto &Pixel: Img.m_vData) {
		if(Pixel.ubR == 0xFF && Pixel.ubG == 0x00 && Pixel.ubB == 0xFF) {
			continue;
		}
		int MinErrorColorIdx = 0;
		int MinError = 0xFFFF * 3;
		for(int idx = 0; idx < 64; ++idx) {
			const auto &Color = vColors[idx];
			auto DeltaR = ((int)Pixel.ubR - (int)Color.ubR);
			auto DeltaG = ((int)Pixel.ubG - (int)Color.ubG);
			auto DeltaB = ((int)Pixel.ubB - (int)Color.ubB);
			auto ColorError = DeltaR*DeltaR + DeltaG*DeltaG + DeltaB*DeltaB;
			if(ColorError < MinError) {
				MinError = ColorError;
				MinErrorColorIdx = idx;
			}
		}
		Pixel = vColors[MinErrorColorIdx];
	}
}

void tryAfterMod(
	tChunkyBitmap &Img, int64_t &ErrorInOut, const std::vector<tRgb> &vColors,
	std::vector<tRgb> &vColorsBest, uint8_t ubColorIdx,
	std::function<bool(tRgb &ColorToMod)> ModOp
)
{
	auto ColorBest = vColors[ubColorIdx];
	auto vColorsTry = vColors;
	auto &ColorNew = vColorsTry[ubColorIdx];
	if(ModOp(ColorNew)) {
		vColorsTry[ubColorIdx + 32] = tRgb(
			ColorNew.ubR >> 1, ColorNew.ubG >> 1, ColorNew.ubB >> 1
		);
		auto ErrorTry = calcError(Img, vColorsTry);
		if(ErrorTry < ErrorInOut) {
			vColorsBest[ubColorIdx] = vColorsTry[ubColorIdx];
			vColorsBest[ubColorIdx + 32] = vColorsTry[ubColorIdx + 32];
			ErrorInOut = ErrorTry;
		}
	}
}

int main(int lArgCount, char *pArgs[])
{
	auto ImgIn = tChunkyBitmap::fromPng("../assets/lv2_world1.png");

	// Initial palette
	std::vector<tRgb> vColors = {
		tRgb( 34,  17,   0), tRgb( 51,  85,   0), tRgb(102, 136,  51), tRgb(153, 187, 102),
		tRgb(204, 238, 153), tRgb(  0,  34,  51), tRgb( 51,  85, 102), tRgb(102, 136, 153),
		tRgb(153, 187, 204), tRgb(204, 238, 255), tRgb( 51,   0,   0), tRgb(102,  34,   0),
		tRgb(153,  68,  17), tRgb(204, 102,  34), tRgb(255, 136,  51), tRgb(255, 170,  68),
		tRgb(255, 204,  85), tRgb(255,  51,   0), tRgb(204,  34,   0), tRgb(170,  17,   0),
		tRgb(119,   0,   0), tRgb(  0,  85, 119), tRgb( 34, 119, 153), tRgb( 68, 153, 187),
		tRgb(102, 187, 221), tRgb(136, 221, 255), tRgb( 17,   0,   0), tRgb( 51,  34,  17),
		tRgb(102,  68,  34), tRgb(153, 102,  51), tRgb(204, 136,  68), tRgb(255, 170,  85),
	};
	vColors.resize(64);
	for(uint8_t i = 0; i < 32; ++i) {
		const auto &Bright = vColors[i];
		vColors[i + 32] = tRgb(Bright.ubR >> 1, Bright.ubG >> 1, Bright.ubB >> 1);
	}

	// Error of previous iteration
	auto Error = calcError(ImgIn, vColors);
	auto PrevError = Error;
	int64_t Delta, PrevDelta;
	fmt::print("Initial error: {:9d}\n", Error);
	// vColors are colors of previous iteration

	int Iter = 0;
	do {
		auto Start = std::chrono::system_clock::now();
		// Best colors deduced in current iteration.
		auto vColorsBest = vColors;

		// Try to modify each color
		for(uint8_t i = 0; i < 32; ++i) {
			// Always start with palette of previous iteration, modify only one color at once
			auto vColorsTry = vColors;

			// Do the search in comparison to error of palette from last iteration
			auto ErrorTry = Error;

			// Modify red: -
			tryAfterMod(ImgIn, ErrorTry, vColors, vColorsTry, i, [](tRgb &ColorNew) {
				if(ColorNew.ubR > 0) {
					ColorNew.ubR -= 17;
					return true;
				}
				return false;
			});

			// Modify red: +
			tryAfterMod(ImgIn, ErrorTry, vColors, vColorsTry, i, [](tRgb &ColorNew) {
				if(ColorNew.ubR < 255) {
					ColorNew.ubR += 17;
					return true;
				}
				return false;
			});

			// Modify green: -
			tryAfterMod(ImgIn, ErrorTry, vColors, vColorsTry, i, [](tRgb &ColorNew) {
				if(ColorNew.ubG > 0) {
					ColorNew.ubG -= 17;
					return true;
				}
				return false;
			});

			// Modify green: +
			tryAfterMod(ImgIn, ErrorTry, vColors, vColorsTry, i, [](tRgb &ColorNew) {
				if(ColorNew.ubG < 255) {
					ColorNew.ubG += 17;
					return true;
				}
				return false;
			});

			// Modify blue: -
			tryAfterMod(ImgIn, ErrorTry, vColors, vColorsTry, i, [](tRgb &ColorNew) {
				if(ColorNew.ubB > 0) {
					ColorNew.ubB -= 17;
					return true;
				}
				return false;
			});

			// Modify blue: +
			tryAfterMod(ImgIn, ErrorTry, vColors, vColorsTry, i, [](tRgb &ColorNew) {
				if(ColorNew.ubB < 255) {
					ColorNew.ubB += 17;
					return true;
				}
				return false;
			});

			if(ErrorTry > Error) {
				fmt::print("try bigger than start!\n");
			}

			// Move the final color to the palette of current iteration
			vColorsBest[i] = vColorsTry[i];
			vColorsBest[i + 32] = vColorsTry[i + 32];
		}

		// Error from colors deduced in current iteration.
		PrevError = Error;
		PrevDelta = Delta;
		Error = calcError(ImgIn, vColorsBest);
		Delta = PrevError - Error;
		vColors = vColorsBest;
		auto Now = std::chrono::system_clock::now();
		auto Elapsed = std::chrono::duration_cast<std::chrono::seconds>(Now - Start).count();
		++Iter;
		fmt::print(
			FMT_STRING("Iteration {:2d} Error: {:9d}, Delta: {:10d}, Time: {}s\n"),
			Iter, Error, Delta, Elapsed
		);
	} while(Iter < 100 && (Delta != 0) && (Delta != -PrevDelta));

	fmt::print(FMT_STRING("Final Error: {}\n"), Error);
	for(int i = 0; i < 32; ++i) {
		fmt::print(
			FMT_STRING("Color {}: {} {} {}\n"),
			i, vColors[i].ubR, vColors[i].ubG, vColors[i].ubB
		);
	}

	replaceColors(ImgIn, vColors);
	ImgIn.toPng("out.png");
	return EXIT_SUCCESS;
}
