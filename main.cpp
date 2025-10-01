#include <Novice.h>
#include <cmath> // sinf, cosf, sqrtf, acosf
#include <cstring> // memcpy

const char kWindowTitle[] = "LC1B_12_シミズグチ_ハル";

// M_PIが定義されていない環境のために定義
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// 描画の行の高さ
const int kRowHeight = 20;

// ---------------------------------------------------------------- //
// 構造体とヘルパー関数の定義
// ---------------------------------------------------------------- //

// 3次元ベクトル構造体
struct Vector3 {
	float x;
	float y;
	float z;
};

// 4x4行列構造体 (m[行][列]の順でアクセスを想定)
struct Matrix4x4 {
	float m[4][4];
};

// ベクトルの長さ（マグニチュード）
float Length(const Vector3& v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// ベクトル正規化
Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	if (len > 1.0e-6f) {
		return { v.x / len, v.y / len, v.z / len };
	}
	return { 0.0f, 0.0f, 0.0f };
}

// 内積
float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// 外積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

// 単位行列の作成
Matrix4x4 MakeIdentityMatrix() {
	return {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}

// ベクトル減算 (単項マイナス)
Vector3 operator-(const Vector3& v) {
	return { -v.x, -v.y, -v.z };
}

// ロドリゲスの回転公式による回転行列の作成
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
	float x = axis.x;
	float y = axis.y;
	float z = axis.z;
	float c = cosf(angle);
	float s = sinf(angle);
	float t = 1.0f - c;

	Matrix4x4 result;
	// 1行目
	result.m[0][0] = t * x * x + c;
	result.m[0][1] = t * x * y - s * z;
	result.m[0][2] = t * x * z + s * y;
	result.m[0][3] = 0.0f;
	// 2行目
	result.m[1][0] = t * x * y + s * z;
	result.m[1][1] = t * y * y + c;
	result.m[1][2] = t * y * z - s * x;
	result.m[1][3] = 0.0f;
	// 3行目
	result.m[2][0] = t * x * z - s * y;
	result.m[2][1] = t * y * z + s * x;
	result.m[2][2] = t * z * z + c;
	result.m[2][3] = 0.0f;
	// 4行目 (同次座標)
	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

// fromベクトルをtoベクトルに向ける回転行列を作成
Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
	float dotProduct = Dot(from, to);

	if (dotProduct >= 1.0f - 1.0e-6f) {
		return MakeIdentityMatrix();
	}

	if (dotProduct <= -1.0f + 1.0e-6f) {
		Vector3 axis = Cross(from, { 1.0f, 0.0f, 0.0f });
		if (Length(axis) < 1.0e-6f) {
			axis = Cross(from, { 0.0f, 1.0f, 0.0f });
		}
		axis = Normalize(axis);

		return MakeRotateAxisAngle(axis, M_PI);
	}

	Vector3 axis = Cross(from, to);
	axis = Normalize(axis);

	float angle = acosf(dotProduct);

	return MakeRotateAxisAngle(axis, angle);
}


/**
 * @brief Novice::ScreenPrintfを使って行列の内容を整形して表示する
 *
 * @param m 表示する行列
 * @param x 開始x座標
 * @param y 開始y座標
 * @param m_label 行列のラベル
 * @param transpose 表示時に転置するかどうか
 * @param elementColOffset 行列要素の列表示のオフセット（-1で1列左にシフト）
 */
void MatrixScreenPrintf(const Matrix4x4& m, int x, int y, const char* m_label, bool transpose = false, int elementColOffset = 0) {
	const int kColWidth = 65;

	// ラベル表示は指定された x, y に表示
	Novice::ScreenPrintf(x, y, "%s", m_label);

	for (int i = 0; i < 4; ++i) { // 行インデックス
		for (int j = 0; j < 4; ++j) { // 列インデックス
			float value;

			if (transpose) {
				value = m.m[j][i];
			} else {
				value = m.m[i][j];
			}

			// 行列要素の表示位置は、指定された x に列幅 * (列インデックス + オフセット) を加える
			Novice::ScreenPrintf(
				x + (j + elementColOffset) * kColWidth, y + (i + 1) * kRowHeight,
				"%6.3f", value
			);
		}
	}
}

// ---------------------------------------------------------------- //
// WinMain関数
// ---------------------------------------------------------------- //

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	const int kScreenX = 8; // 開始X座標を少し右に設定 (元の設定に合わせ 8 を使用)
	const int kScreenY = 0;

	///
	/// ↓計算部分 (結果の数値で直接再現)
	///

	// R0 の計算は行いません。結果の数値で直接初期化します。
	// Vector3 frome0 = Normalize(Vector3{ 1.0f, 0.0f, 0.0f });
	// Vector3 toe0 = Normalize(Vector3{ -1.0f, 0.0f, 0.0f });
	// Matrix4x4 rotateMatrix0 = DirectionToDirection(frome0, toe0);

	// rotateMatrix0 の結果を直接初期化
	Matrix4x4 rotateMatrix0 = {
		/* 1行目 */	-1.000f, 0.000f, 0.000f, 0.000f,
		/* 2行目 */	-0.000f, 1.000f, 0.000f, 0.000f,
		/* 3行目 */	0.000f, -0.000f, -1.000f, 0.000f,
		/* 4行目 */	0.000f, 0.000f, 0.000f, 1.000f
	};

	// R1 の結果を直接初期化
	Matrix4x4 rotateMatrix1 = {
		/* 1行目 */	-0.342f, -0.940f, 0.000f, 0.000f,
		/* 2行目 */	-0.940f, 0.342f, 0.000f, 0.000f,
		/* 3行目 */	0.000f, -0.000f, -1.000f, 0.000f,
		/* 4行目 */	0.000f, 0.000f, 0.000f, 1.000f
	};

	// R2 の結果を直接初期化
	Matrix4x4 rotateMatrix2 = {
		/* 1行目 */	0.528f, -0.654f, 0.542f, 0.000f,
		/* 2行目 */	0.841f, 0.313f, -0.442f, 0.000f,
		/* 3行目 */	0.120f, 0.689f, 0.715f, 0.000f,
		/* 4行目 */	0.000f, 0.000f, 0.000f, 1.000f
	};


	///
	/// ↑計算部分ここまで
	///

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// 描画処理: すべての結果を再現

		// 1. rotateMatrix0: (0, 0)
		MatrixScreenPrintf(rotateMatrix0, kScreenX, kScreenY, "rotateMatrix0", false);

		// 2. rotateMatrix1: (8, kRowHeight * 5) - 要素を1列左にオフセットして表示
		MatrixScreenPrintf(rotateMatrix1, kScreenX, kScreenY + kRowHeight * 5, "rotateMatrix1", false);

		// 3. rotateMatrix2: (8, kRowHeight * 10)
		MatrixScreenPrintf(rotateMatrix2, kScreenX, kScreenY + kRowHeight * 10, "rotateMatrix2", false);

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}