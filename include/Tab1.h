#ifndef TAB1_H
#define TAB1_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/scrolwin.h>

class Tab2Panel;  // 前方宣言

class Tab1Panel : public wxScrolledWindow {
public:
    Tab1Panel(wxWindow* parent);
    void SetCharacterData(const wxString& name, const wxString& species, const wxString& feature, const std::vector<int>& data);
    void UpdateAttribute(wxCommandEvent& event);
    void Updatebonus(wxCommandEvent& event);
    void Updateexp(wxCommandEvent& event);
    void Updateallexp(wxCommandEvent& event);
    void OnCompareValues(wxCommandEvent& event);
    void Updatelevel(wxCommandEvent& event);
    void Updatehp(wxCommandEvent& event);
    void OnSpinCtrlChange(wxCommandEvent& event);
    void abilityChange(wxCommandEvent& event);
    bool SaveToJson(const wxString& filename);
    bool LoadFromJson(const wxString& filename);
private:
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
    wxTextCtrl* rankTextBox; // 冒険者ランク
    wxTextCtrl* bornTextBoxes[3];  // 技, 体, 心
    wxTextCtrl* abilityTextBoxes[6];  // A, B, C, D, E, F
    wxTextCtrl* attributeTextBoxes[6];  // 器用, 敏捷, 筋力, 生命, 知力, 精神
    wxSpinCtrl* growthSpinCtrl[6]; // 成長
    wxSpinCtrl* increaseSpinCtrl[6]; // 増強
    wxTextCtrl* bonusTextBoxes[6]; // ボーナス
    wxTextCtrl* hpTextBoxes[4]; // HP, MP, 抵抗力
    wxSpinCtrl* hitSpinCtrl[4]; //命中力、追加D,、回避力、防護点
    wxSpinCtrl* packSpinCtrl[7]; //判定パッケージ
    wxTextCtrl* moveTextBox; // 移動力
    wxTextCtrl* fullmoveTextBox; // 全力移動
    wxTextCtrl* restmoveTextBox; // 制限移動

    wxTextCtrl* techTextBoxes[6]; // 技能
    wxRadioButton* tableradioA[6]; //技能テーブルA,B
    wxRadioButton* tableradioB[6];
    wxSpinCtrl* techlevelSpinCtrl[6]; // 技能レベル
    wxTextCtrl* expTextBoxes[6]; // 使用経験値
    wxTextCtrl* levelTextBox; // 冒険者レベル
    wxSpinCtrl* getexpSpinCtrl; // 取得経験値
    wxTextCtrl* allexpTextBox; // 合計使用経験値
    wxStaticText* warningText; // 経験値警告

    wxTextCtrl* talentTextBoxes[8]; // 戦闘特技
    wxTextCtrl* abstTextBoxes[8]; // 効果概要
    wxTextCtrl* freetalentTextBox; // 自動習得特技
    wxTextCtrl* freeabstTextBox; // 自動習得効果
    wxTextCtrl* magictalentTextBox; //よく使う魔法など
    wxTextCtrl* battleTextBox; //戦闘効果メモ
    wxTextCtrl* magicabstTextBox; // MP, 効果概要
    wxTextCtrl* enhanceskillTextBoxes1[8]; //練技1
    wxTextCtrl* enhanceskillTextBoxes2[7]; //練技2
    wxTextCtrl* otherTextBox; // 経歴、その他メモなど
    wxTextCtrl* langTextBoxes[10]; // 使用言語
    wxCheckBox* speakcheckBoxes[10]; //会話
    wxCheckBox* readcheckBoxes[10]; //読文

    wxTextCtrl* weaponnameTextBoxes[3]; // 武器名
    wxTextCtrl* weaponuseTextBoxes[6]; // 用法
    wxTextCtrl* weaponstrTextBoxes[6]; // 必筋
    wxTextCtrl* weaponhitTextBoxes[6]; // 命中力
    wxTextCtrl* weapondmTextBoxes[6]; // 威力
    wxTextCtrl* weaponcritTextBoxes[6]; // C値
    wxTextCtrl* weaponaddTextBoxes[6]; // 追加D
    wxTextCtrl* weaponotherTextBoxes[3]; // 備考


    wxTextCtrl* armornameTextBoxes[3]; // 防具名
    wxTextCtrl* armorstrTextBoxes[3]; // 必筋
    wxTextCtrl* armorevadeTextBoxes[3]; // 回避修正
    wxTextCtrl* armorblockTextBoxes[3]; // 防護点
    wxTextCtrl* armorotherTextBoxes[3]; // 備考

    wxSpinCtrl* poketSpinCtrl; // 所持金
    wxTextCtrl* debtTextBox; // 預金/借金
    wxSpinCtrl* glorySpinCtrl; // 名誉点
    wxTextCtrl* acesTextBoxes[12]; // 各部の装飾品
    wxTextCtrl* acesTextBoxes2[12]; // 各部の装飾品の効果説明
    wxTextCtrl* bagTextBox; // 所持アイテムなど
    
};

#endif // TAB1_H
