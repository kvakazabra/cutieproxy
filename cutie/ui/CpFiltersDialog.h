#pragma once
#include <QDialog>
#include <QGroupBox>

#include "../core/QProxyItem.h"
#include "../core/Filter.h"

#include <unordered_map>
#include <memory>

#include <Windows.h>

namespace Ui {
	class FiltersDialog;
}

class CpFiltersDialog : public QDialog {
	Q_OBJECT
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
	static std::set<ProcessIdFilter::TProcessId> convertStringToIds(const QString& input);

	TFilterType currentFilterType() const;
	TFilterType filterTypeByObject(const std::shared_ptr<iphelper::filter_interface>& filter);
Q_SIGNALS:
	void addProcessIdSignal(ProcessIdFilter::TProcessId id);
private Q_SLOTS:
	void filterTypeChanged();
	void addProcessId(ProcessIdFilter::TProcessId id);
private:
	Ui::FiltersDialog* m_Ui{ };
	QProxyItem* m_Proxy{ };
	QString m_OldWindowTitle{ };

	std::unordered_map<TFilterType, QGroupBox*> m_FilterTypeToLayout{ };
};