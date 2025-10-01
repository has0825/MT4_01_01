#include <Novice.h>
#include <cmath>
#include <iostream>
#include <iomanip>


const char kWindowTitle[] = "LC1B_12_シミズグチ_ハル";
// 実行環境に合わせて定義
const float kRowHeight = 20.0f; // Novice環境に合わせて行間隔を調整
const float PI = 3.14159265358979323846f;

// ------------------------------
// 構造体の定義
// ------------------------------

struct Vector3 {
    float x;
    float y;
    float z;
};

struct Matrix4x4 {
    // 行優先 [行][列] の順で格納
    float m[4][4];
};

struct Quaternion {
    // x, y, zがベクトル部、wがスカラー部
    float x;
    float y;
    float z;
    float w;
};

// ------------------------------
// 補助関数 (数学)
// ------------------------------

/**
 * @brief ベクトルの長さを計算する
 */
float Length(const Vector3& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

/**
 * @brief ベクトルを正規化する
 */
Vector3 Normalize(const Vector3& v) {
    float len = Length(v);
    if (len != 0.0f) {
        return { v.x / len, v.y / len, v.z / len };
    }
    return { 0.0f, 0.0f, 0.0f };
}

/**
 * @brief クォータニオンの共役を計算する (q* = (-x, -y, -z, w))
 */
Quaternion Conjugate(const Quaternion& q) {
    return { -q.x, -q.y, -q.z, q.w };
}

/**
 * @brief クォータニオンの乗算 $q_1 q_2$ を計算する
 */
Quaternion Multiply(const Quaternion& q1, const Quaternion& q2) {
    Quaternion result{};
    result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return result;
}

/**
 * @brief ベクトルを4x4行列で変換する (v' = M * v)
 * @details 4x4行列の左上3x3部分のみを使用し、回転変換を行う
 * @param vector 変換対象のベクトル
 * @param matrix 変換行列 (4x4, M[行][列])
 * @return 変換後のベクトル
 */
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
    Vector3 result{};
    Vector3 v_in = vector;

    // v'x = M[0][0]*vx + M[0][1]*vy + M[0][2]*vz
    // v'y = M[1][0]*vx + M[1][1]*vy + M[1][2]*vz
    // v'z = M[2][0]*vx + M[2][1]*vy + M[2][2]*vz
    result.x = v_in.x * matrix.m[0][0] + v_in.y * matrix.m[0][1] + v_in.z * matrix.m[0][2];
    result.y = v_in.x * matrix.m[1][0] + v_in.y * matrix.m[1][1] + v_in.z * matrix.m[1][2];
    result.z = v_in.x * matrix.m[2][0] + v_in.y * matrix.m[2][1] + v_in.z * matrix.m[2][2];

    return result;
}

// ------------------------------
// 主要な関数
// ------------------------------

/**
 * @brief 任意軸回転を表す単位クォータニオンを生成する
 * * Q = (sin(angle/2) * axis_norm.x, sin(angle/2) * axis_norm.y, sin(angle/2) * axis_norm.z, cos(angle/2))
 * * @param axis 回転軸
 * @param angle 回転角度 (ラジアン)
 * @return 単位クォータニオン
 */
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
    Vector3 normalizedAxis = Normalize(axis);
    float halfAngle = angle / 2.0f;
    float s = std::sin(halfAngle);
    float c = std::cos(halfAngle);

    Quaternion result{};
    result.x = normalizedAxis.x * s;
    result.y = normalizedAxis.y * s;
    result.z = normalizedAxis.z * s;
    result.w = c;

    return result;
}

/**
 * @brief ベクトルをQuaternionで回転させた結果のベクトルを求める
 * * V' = Q * V_pure * Q*
 * * @param vector 回転対象のベクトル
 * @param quaternion 回転を表す単位クォータニオン
 * @return 回転後のベクトル
 */
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
    // ベクトルを純粋クォータニオンとして表現: v_pure = (x, y, z, 0)
    Quaternion v_pure = { vector.x, vector.y, vector.z, 0.0f };

    // 共役クォータニオンを計算
    Quaternion q_conjugate = Conjugate(quaternion);

    // 最終積: rotated_pure = q * v_pure * q_conjugate
    Quaternion rotated_pure = Multiply(Multiply(quaternion, v_pure), q_conjugate);

    // 結果のベクトル部を取り出す
    return { rotated_pure.x, rotated_pure.y, rotated_pure.z };
}

/**
 * @brief Quaternionから回転行列を求める (4x4)
 * @details Q = (x, y, z, w) から、ベクトルを左から乗算する際の標準的な回転行列を生成
 * @param q 回転を表す単位クォータニオン
 * @return 4x4回転行列
 */
Matrix4x4 MakeRotateMatrix(const Quaternion& q) {
    Matrix4x4 result{};

    float x2 = q.x * q.x;
    float y2 = q.y * q.y;
    float z2 = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    // 1行目 (標準的な v' = Mv の行列 M)
    result.m[0][0] = 1.0f - 2.0f * y2 - 2.0f * z2;
    result.m[0][1] = 2.0f * xy - 2.0f * wz;
    result.m[0][2] = 2.0f * xz + 2.0f * wy;
    result.m[0][3] = 0.0f;

    // 2行目
    result.m[1][0] = 2.0f * xy + 2.0f * wz;
    result.m[1][1] = 1.0f - 2.0f * x2 - 2.0f * z2;
    result.m[1][2] = 2.0f * yz - 2.0f * wx;
    result.m[1][3] = 0.0f;

    // 3行目
    result.m[2][0] = 2.0f * xz - 2.0f * wy;
    result.m[2][1] = 2.0f * yz + 2.0f * wx;
    result.m[2][2] = 1.0f - 2.0f * x2 - 2.0f * y2;
    result.m[2][3] = 0.0f;

    // 4行目 (並進成分)
    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}

// ------------------------------
// 表示関数 (Novice::ScreenPrintfを使用)
// ------------------------------

/**
 * @brief クォータニオンを表示する (小数点以下2桁)
 */
void QuaternionScreenprintf(float x, float y, const Quaternion& q, const char* label) {
    // 小数点以下2桁に修正
    Novice::ScreenPrintf((int)x, (int)y, "%.2f %.2f %.2f %.2f %s", q.x, q.y, q.z, q.w, label);
}

/**
 * @brief 行列を表示する (転置して表示し、ご要望の出力形式に合わせる)
 */
void MatrixScreenPrintf(float x, float y, const Matrix4x4& m, const char* label) {
    // ラベル表示
    Novice::ScreenPrintf((int)x, (int)y, "%s", label);

    // 行列要素表示 (4x4)
    // 画面出力の行 i は、行列 M の列 i に対応する要素 (M^T の行 i) を表示する
    for (int i = 0; i < 4; ++i) { // i は画面上の行インデックス
        // Novice::ScreenPrintf(x, y, "M[0][i] M[1][i] M[2][i] M[3][i]");
        Novice::ScreenPrintf((int)x, (int)(y + kRowHeight * (i + 1)),
            "%.3f %.3f %.3f %.3f",
            m.m[0][i], m.m[1][i], m.m[2][i], m.m[3][i]);
    }
}

/**
 * @brief ベクトルを表示する
 */
void VectorScreenPrintf(float x, float y, const Vector3& v, const char* label) {
    Novice::ScreenPrintf((int)x, (int)y, "%.2f %.2f %.2f %s", v.x, v.y, v.z, label);
}



// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    // --- テストケース ---
    Quaternion rotation = MakeRotateAxisAngleQuaternion(Normalize(Vector3{ 1.0f, 0.4f, -0.2f }), 0.45f);
    Vector3 pointy = { 2.1f, -0.9f, 1.3f };
    Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
    Vector3 rotateByQuaternion = RotateVector(pointy, rotation);
    Vector3 rotateByMatrix = Transform(pointy, rotateMatrix);

    // ウィンドウの×ボタンが押されるまでループ
    while (Novice::ProcessMessage() == 0) {
        // フレームの開始
        Novice::BeginFrame();

        // キー入力を受け取る
        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

        ///
        /// ↓更新処理ここから
        ///

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        // 描画結果 (ご要望の形式に修正)
        // 0.20 0.08 -0.04 0.97 : rotation
        QuaternionScreenprintf(0, kRowHeight * 0, rotation, ": rotation");

        // rotateMatrix (ラベル+4行, 転置して表示)
        MatrixScreenPrintf(0, kRowHeight * 1, rotateMatrix, "rotateMatrix");

        // 2.15 -1.44 0.46 : rotateByQuaternion
        VectorScreenPrintf(0, kRowHeight * 6, rotateByQuaternion, ": rotateByQuaternion");

        // 2.15 -1.44 0.46 : rotateByMatrix
        VectorScreenPrintf(0, kRowHeight * 7, rotateByMatrix, ": rotateByMatrix");


        ///
        /// ↑描画処理ここまで
        ///

        // フレームの終了
        Novice::EndFrame();

        // ESCキーが押されたらループを抜ける
        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
            break;
        }
    }

    // ライブラリの終了
    Novice::Finalize();
    return 0;
}