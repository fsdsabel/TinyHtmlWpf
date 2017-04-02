#pragma once
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows;
using namespace System::Windows::Media;


interface class IFinderMatchVisualHelper
{
	/// <summary>
	/// Does this item match the input visual item
	/// </summary>
	/// <param name="item">Item to check </param>
	/// <returns>True if matched, else false</returns>
	bool DoesMatch(Visual^ item);

	/// <summary>
	/// Property that defines if we should stop walking the tree after the first match is found
	/// </summary>
	property bool StopAfterFirst;	
};

template <class T>
ref class TypeFinder : IFinderMatchVisualHelper {
public:	
	virtual bool DoesMatch(Visual^ item) {
		return dynamic_cast< T >(item) != nullptr;
	}
	virtual property bool StopAfterFirst;
};

ref class TreeHelpers
{
public:
	static void FindDownInTree(List<FrameworkElement^>^ lst, Visual^ parent, Visual^ ignore, IFinderMatchVisualHelper^ helper);
	static void FindUpInTree(List<FrameworkElement^>^ lst, Visual^ parent, Visual^ ignore, IFinderMatchVisualHelper^ helper);
};

