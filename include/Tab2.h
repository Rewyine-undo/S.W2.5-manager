#ifndef TAB2_H
#define TAB2_H

class Tab1Panel;  // 前方宣言

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/scrolwin.h>
#include <wx/grid.h>
#include <wx/notebook.h>
#include <map>
#include <vector>
#include <string>

struct TrialResult {
    std::string name;
    std::string species; // 種族
    std::string feature; // 種族特性

    int impurity; // 穢れ
    int born[3]; // 技、体、心
    int ability[6]; // A~F
    int attributes[6]; // 技,体,心 + A~F の能力値
    int bonuses[6]; // 能力値ボーナス
    int total; // 能力値合計
    int pretotal; // ダイス目合計
    int dicenumber; // ダイス数
    int diceeffect; // 固定値によるダイス平均への補正
    float average; // 能力値平均
    

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
    Tab2Panel(wxWindow* parent, wxNotebook* notebook, Tab1Panel* tab1Panel); // 修正済み
    ~Tab2Panel();

private:
    wxTextCtrl* nameTextBox;
    wxListBox* speciesListBox;
    std::map<wxString, wxGrid*> speciesTables; //生まれの表
    std::map<wxString, std::vector<std::vector<wxString>>> speciesData;    // 生まれのデータ

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
