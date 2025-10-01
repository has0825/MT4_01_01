#define _CRT_SECURE_NO_WARNINGS
#include <Novice.h>
#include <cmath> 
#include <cstdio> 

const char kWindowTitle[] = "LE2B_11_シミズグチ_ハル";

// 四元数構造体
struct Quaternion {
    float x; // 虚部 i
    float y; // 虚部 j
    float z; // 虚部 k
    float w; // 実部
};

// 四元数の乗算
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
    Quaternion result;
    // 実部 w
    result.w = lhs.w * rhs.w - (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
    // 虚部 x
    result.x = lhs.w * rhs.x + lhs.x * rhs.w + (lhs.y * rhs.z - lhs.z * rhs.y);
    // 虚部 y
    result.y = lhs.w * rhs.y + lhs.y * rhs.w + (lhs.z * rhs.x - lhs.x * rhs.z);
    // 虚部 z
    result.z = lhs.w * rhs.z + lhs.z * rhs.w + (lhs.x * rhs.y - lhs.y * rhs.x);
    return result;
}

// 単位Quaternionを返す
Quaternion IdentityQuaternion() {
    return { 0.0f, 0.0f, 0.0f, 1.0f };
}

// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& quaternion) {
    return { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
}

// Quaternion norm（ノルム）
float Norm(const Quaternion& quaternion) {
    return std::sqrt(quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z +
        quaternion.w * quaternion.w);
}

// 正規化したQuaternion を返す
Quaternion Normalize(const Quaternion& quaternion) {
    float norm = Norm(quaternion);
    if (norm < 1.0e-6f) {
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }
    float invNorm = 1.0f / norm;
    return { quaternion.x * invNorm,
            quaternion.y * invNorm,
            quaternion.z * invNorm,
            quaternion.w * invNorm };
}

// 逆Quaternion を返す
Quaternion Inverse(const Quaternion& quaternion) {
    float normSq = quaternion.x * quaternion.x +
        quaternion.y * quaternion.y +
        quaternion.z * quaternion.z +
        quaternion.w * quaternion.w;

    if (normSq < 1.0e-6f) {
        return { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    float invNormSq = 1.0f / normSq;

    // 共役Quaternionを計算し、ノルムの二乗で割る
    return { -quaternion.x * invNormSq,
            -quaternion.y * invNormSq,
            -quaternion.z * invNormSq,
            quaternion.w * invNormSq };
}

// Quaternionを描画するためのヘルパー関数
void DrawQuaternion(int x, int y, const char* label, const Quaternion& q) {
    char buffer[256];
    
    sprintf(buffer, "%.2f %.2f %.2f %.2f : %s", q.x, q.y, q.z, q.w, label);
    Novice::ScreenPrintf(x, y, "%s", buffer);
}

// float値を描画するためのヘルパー関数
void DrawFloat(int x, int y, const char* label, float value) {
    char buffer[256];
    
    sprintf(buffer, "%.2f : %s", value, label);
    Novice::ScreenPrintf(x, y, "%s", buffer);
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    // 実行結果を生成するためのデータ
    Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
    Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };

    // 演算実行
    Quaternion identity = IdentityQuaternion();
    Quaternion conj = Conjugate(q1);
    Quaternion inv = Inverse(q1);
    Quaternion normal = Normalize(q1);
    Quaternion mul1 = Multiply(q1, q2);
    Quaternion mul2 = Multiply(q2, q1);
    float norm = Norm(q1);

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

        // 実行結果の描画
        int lineHeight = 20;
        int yPos = 10;



        // 演算結果の表示 
        DrawQuaternion(10, yPos, "Identity", identity);
        yPos += lineHeight;
        DrawQuaternion(10, yPos, "Conjugate", conj);
        yPos += lineHeight;
        DrawQuaternion(10, yPos, "Inverse", inv);
        yPos += lineHeight;
        DrawQuaternion(10, yPos, "Normalize", normal);
        yPos += lineHeight;
        DrawQuaternion(10, yPos, "Multiply", mul1);
        yPos += lineHeight;
        DrawQuaternion(10, yPos, "Multiply", mul2);
        yPos += lineHeight;
        DrawFloat(10, yPos, "Norm", norm);
        yPos += lineHeight;

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