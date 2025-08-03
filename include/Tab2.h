#ifndef TAB2_H
#define TAB2_H

class Tab1Panel;  // �O���錾

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/grid.h>
#include <wx/notebook.h>
#include <map>
#include <vector>
#include <string>

struct TrialResult {
    std::string name;
    std::string species; // �푰
    std::string feature; // �푰����

    int impurity; // �q��
    int born[3]; // �Z�A�́A�S
    int ability[6]; // A~F
    int attributes[6]; // �Z,��,�S + A~F �̔\�͒l
    int bonuses[6]; // �\�͒l�{�[�i�X
    int total; // �\�͒l���v
    int pretotal; // �_�C�X�ڍ��v
    int dicenumber; // �_�C�X��
    int diceeffect; // �Œ�l�ɂ��_�C�X���ςւ̕␳
    float average; // �\�͒l����
    

    TrialResult()
        : pretotal(0), total(0), average(0.0), dicenumber(0), diceeffect(0), impurity(0) {
        std::fill(std::begin(born), std::end(born), 0);
        std::fill(std::begin(ability), std::end(ability), 0);
        std::fill(std::begin(attributes), std::end(attributes), 0);
        std::fill(std::begin(bonuses), std::end(bonuses), 0);
    }
};

class Tab2Panel : public wxPanel {
public:
    Tab2Panel(wxWindow* parent, wxNotebook* notebook, Tab1Panel* tab1Panel); // �C���ς�
    ~Tab2Panel();

private:
    wxTextCtrl* nameTextBox;
    wxListBox* speciesListBox;
    std::map<wxString, wxGrid*> speciesTables; //���܂�̕\
    std::map<wxString, std::vector<std::vector<wxString>>> speciesData;    // ���܂�̃f�[�^

    wxStaticText* nameErrorText;
    wxSpinCtrl* skillSpinCtrl;
    wxSpinCtrl* bodySpinCtrl;
    wxSpinCtrl* mindSpinCtrl;
    wxBoxSizer* resultsSizer;
    wxNotebook* notebook;
    Tab1Panel* tab1Panel;
    std::vector<TrialResult> trialResults;

    void InitializeSpeciesData();
    void OnSpeciesSelected(wxCommandEvent & event);
    void OnCreateButtonClicked(wxCommandEvent&);
    void CalculateAttributes(int speciesIndex, int& dicenumber, int& diceeffect, int* ability, int& impurity, std::string& feature);
    void CalculateAbilityValues(TrialResult& result);
    void DisplayResults();
};

#endif // TAB2_H
