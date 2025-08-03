#ifndef TAB1_H
#define TAB1_H

#include <wx/wx.h>
#include <wx/panel.h>
#include <wx/tglbtn.h>
#include <wx/scrolwin.h>

class Tab2Panel;  // �O���錾

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
    wxBoxSizer* sizer; // �x�[�X

    wxBoxSizer* freespaceSizer; 
    wxBoxSizer* placeholderSizer; // ���̑��Z�\�̒ǉ�1
    wxBoxSizer* otherskillSizer; // ���̑��Z�\�̒ǉ�1
    wxBoxSizer* trioSizer;

    wxTextCtrl* nameTextBox; // ���O
    wxTextCtrl* PLTextBox; // PL��
    wxTextCtrl* speciesTextBox; // �푰
    wxTextCtrl* ageTextBox; // �N��
    wxTextCtrl* sexTextBox; // ����
    wxTextCtrl* spfeatureTextBox; // �푰����
    wxTextCtrl* ImpurityTextBox; // �q��
    wxTextCtrl* birthTextBox; // ���܂�
    wxTextCtrl* faithTextBox; // �M��
    wxTextCtrl* rankTextBox; // �`���҃����N
    wxTextCtrl* bornTextBoxes[3];  // �Z, ��, �S
    wxTextCtrl* abilityTextBoxes[6];  // A, B, C, D, E, F
    wxTextCtrl* attributeTextBoxes[6];  // ��p, �q��, �ؗ�, ����, �m��, ���_
    wxSpinCtrl* growthSpinCtrl[6]; // ����
    wxSpinCtrl* increaseSpinCtrl[6]; // ����
    wxTextCtrl* bonusTextBoxes[6]; // �{�[�i�X
    wxTextCtrl* hpTextBoxes[4]; // HP, MP, ��R��
    wxSpinCtrl* hitSpinCtrl[4]; //�����́A�ǉ�D,�A���́A�h��_
    wxSpinCtrl* packSpinCtrl[7]; //����p�b�P�[�W
    wxTextCtrl* moveTextBox; // �ړ���
    wxTextCtrl* fullmoveTextBox; // �S�͈ړ�
    wxTextCtrl* restmoveTextBox; // �����ړ�

    wxTextCtrl* techTextBoxes[6]; // �Z�\
    wxRadioButton* tableradioA[6]; //�Z�\�e�[�u��A,B
    wxRadioButton* tableradioB[6];
    wxSpinCtrl* techlevelSpinCtrl[6]; // �Z�\���x��
    wxTextCtrl* expTextBoxes[6]; // �g�p�o���l
    wxTextCtrl* levelTextBox; // �`���҃��x��
    wxSpinCtrl* getexpSpinCtrl; // �擾�o���l
    wxTextCtrl* allexpTextBox; // ���v�g�p�o���l
    wxStaticText* warningText; // �o���l�x��

    wxTextCtrl* talentTextBoxes[8]; // �퓬���Z
    wxTextCtrl* abstTextBoxes[8]; // ���ʊT�v
    wxTextCtrl* freetalentTextBox; // �����K�����Z
    wxTextCtrl* freeabstTextBox; // �����K������
    wxTextCtrl* magictalentTextBox; //�悭�g�����@�Ȃ�
    wxTextCtrl* battleTextBox; //�퓬���ʃ���
    wxTextCtrl* magicabstTextBox; // MP, ���ʊT�v
    wxTextCtrl* enhanceskillTextBoxes1[8]; //���Z1
    wxTextCtrl* enhanceskillTextBoxes2[7]; //���Z2
    wxTextCtrl* otherTextBox; // �o���A���̑������Ȃ�
    wxTextCtrl* langTextBoxes[10]; // �g�p����
    wxCheckBox* speakcheckBoxes[10]; //��b
    wxCheckBox* readcheckBoxes[10]; //�Ǖ�

    wxTextCtrl* weaponnameTextBoxes[3]; // ���햼
    wxTextCtrl* weaponuseTextBoxes[6]; // �p�@
    wxTextCtrl* weaponstrTextBoxes[6]; // �K��
    wxTextCtrl* weaponhitTextBoxes[6]; // ������
    wxTextCtrl* weapondmTextBoxes[6]; // �З�
    wxTextCtrl* weaponcritTextBoxes[6]; // C�l
    wxTextCtrl* weaponaddTextBoxes[6]; // �ǉ�D
    wxTextCtrl* weaponotherTextBoxes[3]; // ���l


    wxTextCtrl* armornameTextBoxes[3]; // �h�
    wxTextCtrl* armorstrTextBoxes[3]; // �K��
    wxTextCtrl* armorevadeTextBoxes[3]; // ����C��
    wxTextCtrl* armorblockTextBoxes[3]; // �h��_
    wxTextCtrl* armorotherTextBoxes[3]; // ���l

    wxSpinCtrl* poketSpinCtrl; // ������
    wxTextCtrl* debtTextBox; // �a��/�؋�
    wxSpinCtrl* glorySpinCtrl; // ���_�_
    wxTextCtrl* acesTextBoxes[12]; // �e���̑����i
    wxTextCtrl* acesTextBoxes2[12]; // �e���̑����i�̌��ʐ���
    wxTextCtrl* bagTextBox; // �����A�C�e���Ȃ�
    
};

#endif // TAB1_H
