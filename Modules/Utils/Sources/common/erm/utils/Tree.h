#pragma once

#include "erm/utils/Utils.h"

#include <functional>
#include <memory>
#include <vector>

namespace erm {

template<typename ID, typename Payload>
class Tree
{
public:
	using Child = std::unique_ptr<Tree>;
	using Children = std::vector<Child>;

public:
	Tree(ID id, Payload payload)
		: mId(std::forward<ID>(id))
		, mPayload(std::forward<Payload>(payload))
		, mParent(nullptr)
	{}
	~Tree() = default;

	Tree(const Tree&) = delete;
	Tree& operator=(const Tree&) = delete;

	Tree(Tree&& other) noexcept
		: mId(std::move(other.mId))
		, mPayload(std::move(other.mPayload))
		, mParent(std::move(other.mParent))
		, mChildren(std::move(other.mChildren))
	{
		for (Child& child : mChildren)
		{
			child->mParent = this;
		}
	}

	Tree& operator=(Tree&& other) noexcept
	{
		if (&other == this)
			return *this;
		
		*this = std::move(other);
		
		return *this;
	}

	Tree Clone() const
	{
		Tree tree(Utils::Clone(mId), Utils::Clone(mPayload));

		Tree* currentNode = &tree;

		ForEachChildDo(
			[&currentNode](const Tree& node) mutable {
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

	Tree& AddChild(ID id, Payload payload)
	{
		if (Tree* child = GetRoot().Find(id))
		{
			child->SetPayload(std::forward<Payload>(payload));
			return *child;
		}
		Child& child = mChildren.emplace_back(std::make_unique<Tree>(std::forward<ID>(id), std::forward<Payload>(payload)));
		child->mParent = this;
		return *child;
	}

	inline void ForEachDo(
		const std::function<void(Tree&)>& before,
		const std::function<void(Tree&)>& after = nullptr)
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

	inline void ForEachChildDo(
		const std::function<void(Tree&)>& before,
		const std::function<void(Tree&)>& after = nullptr)
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

	inline Tree* Find(const ID& id)
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

	inline const Tree* Find(const ID& id) const
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

	inline void SetPayload(Payload payload) { mPayload = std::forward<Payload>(payload); }
	inline const Payload& GetPayload() const { return mPayload; }
	inline Payload& GetPayload() { return mPayload; }
	
	inline void SetId(ID id) { mId = std::forward<ID>(id); }
	inline const ID& GetId() const { return mId; }
	inline ID& GetId() { return mId; }
	
	inline Tree* GetParent() { return mParent; }
	inline const Children& GetChildren() const { return mChildren; }

private:
	ID mId;
	Payload mPayload;
	Tree* mParent;
	Children mChildren;
};

} // namespace erm
