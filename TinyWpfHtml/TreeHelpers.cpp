#include "stdafx.h"
#include "TreeHelpers.h"

/*TypeFinder::TypeFinder(Type^ type) {
	_type = type;
}*/



/// <summary>
/// Really a helper for FindDownInTree, typically not called directly.
/// </summary>
/// <param name="lst"></param>
/// <param name="parent"></param>
/// <param name="ignore"></param>
/// <param name="helper"></param>
void TreeHelpers::FindDownInTree(List<FrameworkElement^>^ lst, Visual^ parent, Visual^ ignore, IFinderMatchVisualHelper^ helper)
{
	for (int i = 0; i < VisualTreeHelper::GetChildrenCount(parent); i++)
	{
		if (lst->Count > 0 && helper->StopAfterFirst) break;

		auto visual = dynamic_cast<Visual^>(VisualTreeHelper::GetChild(parent, i));

		if (dynamic_cast<FrameworkElement^>(visual))
		{
			dynamic_cast<FrameworkElement^>(visual)->ApplyTemplate();
		}

		if (helper->DoesMatch(visual))
		{
			lst->Add(dynamic_cast<FrameworkElement^>(visual));
		}

		if (lst->Count > 0 && helper->StopAfterFirst) break;

		if (visual != ignore)
		{
			FindDownInTree(lst, visual, ignore, helper);
		}
	}
}

/// <summary>
/// Really a helper to look Up in a tree, typically not called directly.
/// </summary>
/// <param name="lst"></param>
/// <param name="parent"></param>
/// <param name="ignore"></param>
/// <param name="helper"></param>
void TreeHelpers::FindUpInTree(List<FrameworkElement^>^ lst, Visual^ parent, Visual^ ignore, IFinderMatchVisualHelper^ helper)
{
	FrameworkElement^ feCast = dynamic_cast<FrameworkElement^>(parent);
	if (feCast != nullptr)
	{
		FrameworkElement^ feNewParent = dynamic_cast<FrameworkElement^>(VisualTreeHelper::GetParent(feCast));
		if (feNewParent != nullptr) {
			if (helper->DoesMatch(feNewParent)) {
				lst->Add(feNewParent);
				if (helper->StopAfterFirst) {
					return;
				}
			}
			FindUpInTree(lst, feNewParent, ignore, helper);
		}
	}
}

