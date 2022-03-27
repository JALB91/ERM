#pragma once

#include "erm/utils/Utils.h"

#include <functional>
#include <memory>
#include <vector>

namespace erm {

template<typename S, typename T>
class Tree
{
public:
	typedef std::unique_ptr<Tree> Child;
	typedef std::vector<Child> Children;
	typedef S ID;
	typedef T Payload;

public:
	Tree(S id, T payload)
		: mParent(nullptr)
		, mId(std::forward<S>(id))
		, mPayload(std::forward<T>(payload))
	{}
	~Tree() = default;

	Tree(const Tree&) = delete;
	Tree& operator=(const Tree&) = delete;

	Tree(Tree&& other)
		: mParent(std::move(other.mParent))
		, mChildren(std::move(other.mChildren))
		, mId(std::move(other.mId))
		, mPayload(std::move(other.mPayload))
	{
		for (Child& child : mChildren)
		{
			child->mParent = this;
		}
	}

	Tree& operator=(Tree&& other)
	{
		if (&other == this)
			return *this;

		mParent = std::move(other.mParent);
		mChildren = std::move(other.mChildren);
		mId = std::move(other.mId);
		mPayload = std::move(other.mPayload);

		for (Child& child : mChildren)
		{
			child->mParent = this;
		}

		return *this;
	}

	Tree Clone() const
	{
		Tree tree(Utils::Clone(mId), Utils::Clone(mPayload));

		Tree* currentNode = &tree;

		ForEachChildDo([&currentNode](const Tree& node) mutable {
			currentNode = &(currentNode->AddChild(Utils::Clone(node.GetId()), Utils::Clone(node.GetPayload())));
		},
					   [&currentNode](const Tree& /*node*/) mutable {
						   currentNode = currentNode->GetParent();
					   });

		return tree;
	}

	const Tree& GetRoot() const
	{
		const Tree* parent = mParent;
		while (parent)
		{
			if (parent->mParent)
			{
				parent = parent->mParent;
			}
			else
			{
				return *parent;
			}
		}
		return *this;
	}

	Tree& GetRoot()
	{
		Tree* parent = mParent;
		while (parent)
		{
			if (parent->mParent)
			{
				parent = parent->mParent;
			}
			else
			{
				return *parent;
			}
		}
		return *this;
	}

	void AddChild(Child&& node)
	{
#if !defined(NDEBUG)
		const Tree& root = GetRoot();
		node->ForEachDo([&root](const Tree& node) {
			ERM_ASSERT(root.Find(node.GetId()) == nullptr);
		});
#endif
		Child& child = mChildren.emplace_back(std::move(node));
		child->mParent = this;
	}

	Tree& AddChild(S id, T payload)
	{
		if (Tree* child = GetRoot().Find(id))
		{
			child->SetPayload(std::forward<T>(payload));
			return *child;
		}
		Child& child = mChildren.emplace_back(std::make_unique<Tree>(std::forward<S>(id), std::forward<T>(payload)));
		child->mParent = this;
		return *child;
	}

	inline void ForEachDo(const std::function<void(Tree&)>& before, const std::function<void(Tree&)>& after = nullptr)
	{
		if (before)
			before(*this);
		for (Child& child : mChildren)
		{
			child->ForEachDo(before, after);
		}
		if (after)
			after(*this);
	}

	inline void ForEachChildDo(const std::function<void(Tree&)>& before, const std::function<void(Tree&)>& after = nullptr)
	{
		for (Child& child : mChildren)
		{
			if (before)
				before(*child);
			child->ForEachChildDo(before, after);
			if (after)
				after(*child);
		}
	}

	inline Tree* Find(S id)
	{
		if (mId == id)
			return this;

		for (Child& child : mChildren)
		{
			if (Tree* result = child->Find(id))
			{
				return result;
			}
		}

		return nullptr;
	}

	inline const Tree* Find(S id) const
	{
		if (mId == id)
			return this;

		for (const Child& child : mChildren)
		{
			if (const Tree* result = child->Find(id))
			{
				return result;
			}
		}

		return nullptr;
	}

	template<typename Func>
	inline const Tree* FindByPayload(Func function) const
	{
		if (function(mPayload))
			return this;

		for (const Child& child : mChildren)
			if (const Tree* result = child->FindByPayload(function))
				return result;

		return nullptr;
	}

	inline unsigned int GetSize() const
	{
		unsigned int size = 1;
		for (const Child& child : mChildren)
			size += child->GetSize();
		return size;
	}

	inline void SetPayload(T payload) { mPayload = std::forward<T>(payload); }
	inline const T& GetPayload() const { return mPayload; }
	inline T& GetPayload() { return mPayload; }
	inline S GetId() const { return mId; }
	inline void SetId(S id) { mId = id; }
	inline Tree* GetParent() { return mParent; }
	inline const Children& GetChildren() const { return mChildren; }

	inline bool Equal(Tree& other) const
	{
		if (GetId() != other.GetId() || GetPayload() != other.GetPayload())
			return false;
		if (mChildren.size() != other.mChildren.size() || GetSize() != other.GetSize())
			return false;

		bool equals = true;
		for (size_t i = 0; i < mChildren.size(); ++i)
		{
			equals &= mChildren[i]->Equal(*other.mChildren[i]);
			if (!equals)
				break;
		}

		return equals;
	}

private:
	Tree* mParent;
	Children mChildren;
	S mId;
	T mPayload;
};

} // namespace erm
