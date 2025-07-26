#pragma once
#include <QDialog>
#include <QGroupBox>

#include "../core/QProxyItem.h"

#include <unordered_map>
#include <memory>

namespace Ui {
	class FiltersDialog;
}

class CpFiltersDialog : public QDialog {
public:
	CpFiltersDialog(QWidget* parent, QProxyItem* proxy);
	~CpFiltersDialog();
public:
	std::shared_ptr<iphelper::filter_interface> filter() const;
private:
	enum class TFilterType {
		None,
		All,
		Name,
		ProcessId,
	};
private:
	void init();
	void connectSignals();

	TFilterType currentFilterType() const;
	void filterTypeChanged();
	TFilterType filterTypeByObject(const std::shared_ptr<iphelper::filter_interface>& filter);
private:
	Ui::FiltersDialog* m_Ui{ };
	QProxyItem* m_Proxy{ };

	std::unordered_map<TFilterType, QGroupBox*> m_FilterTypeToLayout{ };
};