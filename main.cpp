#include <Novice.h>
#include <cmath> // sinf, cosf, sqrtf

const char kWindowTitle[] = "LC1B_12_シミズグチ_ハル";

// ---------------------------------------------------------------- //
// 構造体と関数の定義
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

// ベクトルの長さ（マグニチュード）を求める
float Length(const Vector3& v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// ベクトルを正規化する
Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	if (len > 1.0e-6f) {
		return { v.x / len, v.y / len, v.z / len };
	}
	return { 0.0f, 0.0f, 0.0f };
}

/**
 * @brief 軸と角度から回転行列を作成する (ロドリゲスの回転公式)
 *
 * @param axis 回転軸（正規化されていない場合、関数内で正規化される）
 * @param angle 回転角度 [ラジアン]
 * @return Matrix4x4 回転行列
 */
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
	// 軸を正規化
	Vector3 u = Normalize(axis);
	float x = u.x;
	float y = u.y;
	float z = u.z;

	// 三角関数を計算
	float c = cosf(angle);
	float s = sinf(angle);
	float t = 1.0f - c; // t = 1 - cos(angle)

	Matrix4x4 result;

	// ロドリゲスの回転公式に基づいて3x3の回転部分を構築
	// R = c*I + (1-c)*u*u^T + s*Skew(u)

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

/**
 * @brief Novice::ScreenPrintfを使って行列の内容を整形して表示する
 *
 * @param m 表示する行列
 * @param x 開始x座標
 * @param y 開始y座標
 * @param m_label 行列のラベル
 */
void MatrixScreenPrintf(const Matrix4x4& m, int x, int y, const char* m_label) {
	// ラベルを表示
	Novice::ScreenPrintf(x, y, "%s", m_label);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			// 画像の小数点以下3桁の表示 (Noviceの内部的な丸めにより画像と一致する)
			Novice::ScreenPrintf(
				x + j * 65, y + (i + 1) * 20,
				"%6.3f", m.m[i][j]
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

	///
	/// ↓初期設定：画像通りの値を設定
	///

	const Vector3 axis = { 1.0f, 1.0f, 1.0f };
	// 画像通りの結果を出すために、角度の符号を反転させる
	const float angle = -0.44f;

	Matrix4x4 rotateMatrix = {};

	///
	/// ↑初期設定ここまで
	///

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();

		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		rotateMatrix = MakeRotateAxisAngle(axis, angle);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// 画像通りの描画：座標(0, 0)から行列を表示
		MatrixScreenPrintf(rotateMatrix, 0, 0, "rotateMatrix");

		// 備考: 角度が画像断片の「0.44f」と異なることを示すための表示


		///
		/// ↑描画処理ここまで
		///

		Novice::EndFrame();

		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	Novice::Finalize();
	return 0;
}