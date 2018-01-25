#pragma once

#include "CWEGraphicResource.h"
#include "CWEMath.h"
#include "CWETypeDefinition.h"
#include "CWEError.h"



namespace CWE
{
class Graphic;


class Motion;


// ポリゴン一般クラス
class Polygon
{
private:
	// 2D表示用の射影変換行列
	static math::Float4x4 m2DProj;

	// リソースクラスにアクセスする為の参照
	Resource &mResource;

	// リソースへのハンドル
	ResourceHandle mHandle;

	// テクスチャ使用スロット
	unsigned mTexSlot;

	//相対姿勢情報リセット
	void ResetPose(PolygonData& data);
	// FKでのボーンの更新
	void FKUpdate(PolygonData& data);
	// 指定ボーンから先の階層のボーンのみをFK更新
	void FKUpdate(PolygonData& data, unsigned boneIndex);
	// IKの適用
	// (FK更新済みである事が前提)
	void IKApply(PolygonData& data);
	// 3D描画
	void Draw3D(const PolygonData& data, bool cull, bool eachMesh, unsigned numMeshes=0)const;
	// 2D描画
	void Draw2D(const PolygonData& data)const;

public:
	// 2D表示用の射影変換行列を取得する
	static const math::Float4x4& Get2DProjection() {return m2DProj;}

	Polygon();
	Polygon(const Polygon &original, bool copyAll);
	Polygon(const Polygon& original);
	Polygon& operator=(const Polygon& original);
	~Polygon();

	// 位置座標
	math::Float3 mPosition;


	// ファイルのロード(新規リソースの生成)
	void Load(const wchar_t *filePath, const FileFormat &format);

	// ハンドルの参照先が存在するか
	bool Is() const {mResource.IsPolygon(mHandle);}

	// 現在のハンドルの参照先が使用可能かどうか
	const Task& LoadResult()const;

	// データのコピー
	void Copy(const Polygon &original, bool copyAll);

	// データの開放
	void Release();

	// 描画
	void Draw(bool cull)const;

	// メッシュごとに描画
	void DrawMesh(unsigned meshNumber, bool cull)const;

	// hlslでのテクスチャスロットのセット
	void SetTexSlot(unsigned slot) {CWE_ASSERT(slot<D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT); mTexSlot =slot;}

	// メッシュデータの取得
	const std::vector<Mesh>& GetMeshData(bool* result) const;

	// メッシュ総数の取得
	unsigned ReadMeshNum()const;

	// メッシュ情報の取得
	const Mesh& ReadMesh(unsigned meshNumber)const;

	// ボーンデータの取得
	const std::vector<Bone>& GetBoneData(bool* result) const;
	std::vector<Bone>& GetBoneData(bool* result);

	// ボーン総数の取得
	unsigned GetBoneNum()const;

	// ボーン情報の取得
	const Bone& GetBone(unsigned boneNumber)const;

	// モーションを適用して再生
	// 戻り値：true=再生中 false=再生終了
	bool MotionUpdate(Motion& motion, float frame);

	// モーションを補間して適用
	void MotionLerp(Motion& from, Motion& to, float t);

	// FKでのボーンの更新
	bool FKApply();

	// IKの適用
//	bool IKApply(); //モーションファイル以外でボーンを動かしたくなったら実装

	// 頂点変換行列の更新
	bool UpdateSkinningMatrix();

};




}//CWE