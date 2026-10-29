//メイン管理タブ
#ifndef TAB1_H
#define TAB1_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/tglbtn.h>
#include <wx/spinctrl.h>
#include <wx/scrolwin.h>
#include <wx/collpane.h>
#include <vector>
#include <array>
#include "json.hpp"

using json = nlohmann::json;

class Tab2Panel;  // 前方宣言

class Tab1Panel : public wxScrolledWindow {
public:
    Tab1Panel(wxWindow* parent);
    // セッション履歴から計算した合計を反映
    void SetSessionTotals(
        long long experience, long long money, long long honor);
    void SetCharacterData(const wxString& name, const wxString& species, const wxString& feature, const std::vector<int>& data);
    void SetGrowthTotals(const std::array<long long, 6>& growthTotals);
    void SetMatecaTotals(
        const std::array<std::array<long long, 4>, 5>& matecaTotals
    );
    void EnsureCombatGrid(); // 戦士技能の表追加
    void AddCombatSkillRow(const wxString& className,
    int level,
    int hit,
    int evade,
    int damage,
    bool showEvade); // 戦士技能の行追加
    void EnsureMagicGrid(); // 魔法使い技能の表追加
    void AddMagicSkillRow(const wxString& className,
    int level,
    int magic,
    int useMagic); // 魔法使い技能の行追加
    void EnsureOtherGrid(); // その他技能の表追加
    void AddOtherSkillRow(const wxString& className,
    int level,
    const wxString& techValue,
    const wxString& movValue,
    const wxString& sageValue); // その他技能の行追加
    void ClearSkillTables(); // 追加行の整理
    void RebuildStatusDetailUI();  // 技能名変更時に、技能用UIを再構築
    // void UpdateSkillEffects(int index); // 自動技能や探索技能など構築
    void RebuildSkillDetailUI();  // 技能名変更時に、技能用UIを再構築
    void UpdateAttribute(wxCommandEvent& event); // 能力値の計算
    void Updatebonus(wxCommandEvent& event); // 能力ボーナスの計算
    void Updateexp(wxCommandEvent& event); // HPや
    void Updateallexp(wxCommandEvent& event);
    void Updatelevel(wxCommandEvent& event);
    void Updatehp(wxCommandEvent& event);

    void OnCompareValues(wxCommandEvent& event);
    void OnSpinCtrlChange(wxCommandEvent& event);
    void abilityChange(wxCommandEvent& event);
    json CreateJsonFromGUI();
    bool SaveToJson(const wxString& filename);
    bool LoadFromJson(const wxString& filename);
    bool CopyToCocofolia();
    
    std::vector<wxTextCtrl*> enhancerTextBoxes;
    std::vector<wxTextCtrl*> bardTextBoxes;
    std::vector<wxTextCtrl*> riderTextBoxes;
    std::vector<wxTextCtrl*> alchemistTextBoxes;
    std::vector<wxCollapsiblePane*> collapsiblePanes;
    void OnExpandAll(wxCommandEvent& event);
    void OnCollapseAll(wxCommandEvent& event);
    void AddExtraTalent(const wxString& talentName);
private:
    wxWindow* skillWindow = nullptr;
    wxWindow* statusWindow = nullptr;
    wxBoxSizer* statusSizer = nullptr;

    wxBoxSizer* sizer; // ベース

    wxBoxSizer* freespaceSizer; 
    wxBoxSizer* placeholderSizer; // その他技能の追加1
    wxBoxSizer* otherskillSizer; // その他技能の追加1
    wxBoxSizer* trioSizer;

    wxTextCtrl* nameTextBox; // 名前
    wxTextCtrl* PLTextBox; // PL名
    wxTextCtrl* speciesTextBox; // 種族
    wxTextCtrl* ageTextBox; // 年齢
    wxTextCtrl* sexTextBox; // 性別
    wxTextCtrl* spfeatureTextBox; // 種族特性
    wxTextCtrl* ImpurityTextBox; // 穢れ
    wxTextCtrl* birthTextBox; // 生まれ
    wxTextCtrl* faithTextBox; // 信仰
    wxChoice* rankChoices; // 冒険者ランク
    wxTextCtrl* bornTextBoxes[3];  // 技, 体, 心
    wxTextCtrl* abilityTextBoxes[6];  // A, B, C, D, E, F
    wxTextCtrl* attributeTextBoxes[6];  // 器用, 敏捷, 筋力, 生命, 知力, 精神
    wxSpinCtrl* growthSpinCtrl[6]; // 成長
    wxSpinCtrl* increaseSpinCtrl[6]; // 増強
    wxTextCtrl* bonusTextBoxes[6]; // ボーナス
    wxTextCtrl* hpTextBoxes[4]; // HP, MP, 抵抗力
    
    wxTextCtrl* moveTextBox; // 移動力
    wxTextCtrl* fullmoveTextBox; // 全力移動
    wxTextCtrl* restmoveTextBox; // 制限移動
    wxSpinCtrl* armorSpinCtrl; // 防護点

    // 技能グループごとの表
    wxStaticText* combatGridTitle = nullptr;
    wxStaticText* magicGridTitle = nullptr;
    wxStaticText* otherGridTitle = nullptr;
    wxGrid* combatGrid = nullptr;
    wxGrid* magicGrid = nullptr;
    wxGrid* otherGrid = nullptr;

    wxTextCtrl* techTextBoxes[6]; // 技能
    wxRadioButton* tableradioA[6]; //技能テーブルA,B
    wxRadioButton* tableradioB[6];
    wxSpinCtrl* techlevelSpinCtrl[6]; // 技能レベル
    wxTextCtrl* expTextBoxes[6]; // 使用経験値
    wxTextCtrl* levelTextBox; // 冒険者レベル
    wxSpinCtrl* getexpSpinCtrl; // 取得経験値
    wxTextCtrl* allexpTextBox; // 合計使用経験値
    wxStaticText* warningText; // 経験値警告

    std::vector<wxTextCtrl*> talentTextBoxes;
    std::vector<wxTextCtrl*> abstTextBoxes;
    std::vector<wxTextCtrl*> extraTalentTextBoxes;
    std::vector<wxTextCtrl*> extraAbstTextBoxes;

    wxBoxSizer* talentSizer = nullptr;
    wxBoxSizer* abstSizer = nullptr;
    wxBoxSizer* extraTalentSizer = nullptr;
    wxBoxSizer* extraAbstSizer = nullptr;

    wxTextCtrl* freetalentTextBox; // 自動習得特技
    wxTextCtrl* freeabstTextBox; // 自動習得効果
    wxTextCtrl* magictalentTextBox; //よく使う魔法など
    wxTextCtrl* battleTextBox; //戦闘効果メモ
    wxTextCtrl* magicabstTextBox; // MP, 効果概要
    wxTextCtrl* enhanceskillTextBoxes1[8]; //練技1
    wxTextCtrl* enhanceskillTextBoxes2[7]; //練技2
    wxTextCtrl* otherTextBox; // 経歴、その他メモなど
    wxTextCtrl* chatTextBox; // チャットパレット
    wxTextCtrl* langTextBoxes[10]; // 使用言語
    wxCheckBox* speakcheckBoxes[10]; //会話
    wxCheckBox* readcheckBoxes[10]; //読文

    wxSpinCtrl* matecaSpinCtrl[5][4]; // マテリアルカード
    wxTextCtrl* potionTextBoxes[6]; // ポーション
    wxSpinCtrl* potionSpinCtrl[6]; 
    wxTextCtrl* ringTextBoxes[6]; // 指輪/腕輪
    wxSpinCtrl* ringSpinCtrl[6];

    wxTextCtrl* weaponnameTextBoxes[3]; // 武器名
    wxTextCtrl* weaponuseTextBoxes[6]; // 用法
    wxTextCtrl* weaponstrTextBoxes[6]; // 必筋
    wxTextCtrl* weaponhitTextBoxes[6]; // 命中力
    wxTextCtrl* weapondmTextBoxes[6]; // 威力
    wxTextCtrl* weaponcritTextBoxes[6]; // C値
    wxChoice* weapontypeChoices[3]; // 武器種
    wxTextCtrl* weaponaddTextBoxes[6]; // 追加D
    wxTextCtrl* weaponotherTextBoxes[3]; // 備考


    wxTextCtrl* armornameTextBoxes[3]; // 防具名
    wxTextCtrl* armorstrTextBoxes[3]; // 必筋
    wxTextCtrl* armorevadeTextBoxes[3]; // 回避修正
    wxTextCtrl* armorblockTextBoxes[3]; // 防護点
    wxChoice* armortypeChoices[3]; // 防具カテゴリ
    wxTextCtrl* armorotherTextBoxes[3]; // 備考

    wxSpinCtrl* poketSpinCtrl; // 所持金
    wxTextCtrl* debtTextBox; // 預金/借金
    wxSpinCtrl* glorySpinCtrl; // 名誉点
    wxTextCtrl* acesTextBoxes[12]; // 各部の装飾品
    wxTextCtrl* acesTextBoxes2[12]; // 各部の装飾品の効果説明
    wxTextCtrl* bagTextBox; // 所持アイテムなど

    // 追加表示UI
    // 技能によって動的に追加するUI
    wxBoxSizer* packageDetailSizer = nullptr;
    wxBoxSizer* skillDetailSizer = nullptr;

};

#endif // TAB1_H
