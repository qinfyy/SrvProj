#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

// 前向声明
class CharacterDesDef;
class HonorDef;
class ChatDef;
class CharGemDef;

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>

class ElementType {
public:
    // 预定义静态实例
    static const ElementType Inherit;
    static const ElementType Aqua;
    static const ElementType Fire;
    static const ElementType Earth;
    static const ElementType Wind;
    static const ElementType Light;
    static const ElementType Dark;
    static const ElementType None;

    // 获取所有实例的列表
    static const std::vector<const ElementType*>& Values();

    // 通过 value 获取 ElementType
    static const ElementType* GetByValue(int value);

    // 成员变量（public，保持 mX 命名）
    int mValue = 0;
    int mSubNoteSkillItemId = 0;
    std::unordered_set<int> mGemAttrTypes;

    // 构造函数
    ElementType(int value, int subNoteSkillItemId = 0, const std::unordered_set<int>& attrTypes = {});

    // 禁用拷贝（枚举应该是单例）
    ElementType(const ElementType&) = delete;
    ElementType& operator=(const ElementType&) = delete;

private:
    // 静态存储
    static std::vector<const ElementType*> msValues;
    static std::unordered_map<int, const ElementType*> msValueMap;

    // 静态初始化
    static void Initialize();
    static bool msInitialized;
};

// 便捷函数
inline const ElementType* GetElementTypeByValue(int value) {
    return ElementType::GetByValue(value);
}

class CharacterDef : public ResBase {
public:
    CharacterDef() = default;
    ~CharacterDef() override = default;

    // 禁用拷贝构造和赋值
    CharacterDef(const CharacterDef&) = delete;
    CharacterDef& operator=(const CharacterDef&) = delete;

    // 移动语义支持
    CharacterDef(CharacterDef&&) noexcept = default;
    CharacterDef& operator=(CharacterDef&&) noexcept = default;

    // Getter 方法 - 只有 Id 需要重写父类虚函数
    int GetId() const override { return mId; }

    // 业务方法
    int GetSkillsUpgradeGroup(int index) const;
    CharGemDef* GetCharGemData(int slotId) const;

    // 生命周期方法
    void OnLoad() override;

	bool LoadFromPb(std::string data) override;

    bool mVisible = false;
    bool mAvailable = false;
    std::string mName;
    int mGrade = 0;
    int mEet = 0;

    int mDefaultSkinId = 0;
    int mAdvanceSkinId = 0;
    int mAdvanceSkinUnlockLevel = 0;

    int mAdvanceGroup = 0;
    std::vector<int> mSkillsUpgradeGroup;

    int mFragmentsId = 0;
    int mTransformQty = 0;

    std::vector<int> mGemSlots;

    CharacterDesDef* mDes = nullptr;
    HonorDef* mHonor = nullptr;
    ElementType mElementType = ElementType::None;
    std::vector<ChatDef*> mChats;

private:
    int mId = 0;
};
