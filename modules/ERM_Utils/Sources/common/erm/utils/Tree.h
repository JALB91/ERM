#pragma once

#include "erm/utils/Utils.h"

#include <erm/log/Assert.h>

#include <functional>
#include <memory>
#include <vector>
#include <type_traits>

namespace erm {

template<typename ID, typename Payload>
class Tree
{
public:
	using Child = Tree;
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
			child.mParent = this;
		}
	}

	Tree& operator=(Tree&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}
		
		mId = std::move(other.mId);
		mPayload = std::move(other.mPayload);
		mParent = std::move(other.mParent);
		mChildren = std::move(other.mChildren);
		
		for (auto& child : mChildren)
		{
			child.mParent = this;
		}
		
		return *this;
	}

	Tree clone() const
	{
		Tree tree(utils::clone(mId), utils::clone(mPayload));

		Tree* currentNode = &tree;

		forEachChildDo(
			[&currentNode](const Tree& node) mutable {
				currentNode = &(currentNode->addChild(utils::clone(node.getId()), utils::clone(node.getPayload())));
			},
			[&currentNode](const Tree& /*node*/) mutable {
				currentNode = currentNode->getParent();
			});

		return tree;
	}

	const Tree& getRoot() const
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

	Tree& getRoot()
	{
		auto* parent = mParent;
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

	void addChild(Child&& node)
	{
#if !defined(ERM_DEBUG)
		const Tree& root = getRoot();
		node.forEachDo([&root](const Tree& node) {
			ERM_ASSERT(root.find(node.getId()) == nullptr);
		});
#endif
		Child& child = mChildren.emplace_back(std::move(node));
		child.mParent = this;
	}

	Tree& addChild(ID id, Payload payload)
	{
		if (Tree* child = getRoot().find(id))
		{
			child->setPayload(std::forward<Payload>(payload));
			return *child;
		}
		auto& child = mChildren.emplace_back(std::forward<ID>(id), std::forward<Payload>(payload));
		child.mParent = this;
		return child;
	}

	inline void forEachDo(
		const std::function<void(Tree&)>& before,
		const std::function<void(Tree&)>& after = nullptr)
	{
		if (before)
		{
			before(*this);
		}
		for (Child& child : mChildren)
		{
			child.forEachDo(before, after);
		}
		if (after)
		{
			after(*this);
		}
	}

	inline void forEachChildDo(
		const std::function<void(Tree&)>& before,
		const std::function<void(Tree&)>& after = nullptr)
	{
		for (Child& child : mChildren)
		{
			if (before)
			{
				before(*child);
			}
			child.forEachChildDo(before, after);
			if (after)
			{
				after(*child);
			}
		}
	}

	inline void forEachDo(
		const std::function<void(const Tree&)>& before,
		const std::function<void(const Tree&)>& after = nullptr) const
	{
		if (before)
		{
			before(*this);	
		}
		for (const auto& child : mChildren)
		{
			child.forEachDo(before, after);
		}
		if (after)
		{
			after(*this);	
		}
	}

	inline void forEachChildDo(
		const std::function<void(const Tree&)>& before,
		const std::function<void(const Tree&)>& after = nullptr) const
	{
		for (const auto& child : mChildren)
		{
			if (before)
			{
				before(child);
			}
			child.forEachChildDo(before, after);
			if (after)
			{
				after(child);	
			}
		}
	}

	inline Tree* find(const ID& id)
	{
		if (mId == id)
		{
			return this;	
		}

		for (auto& child : mChildren)
		{
			auto* result = child.find(id);
			if (result != nullptr)
			{
				return result;
			}
		}

		return nullptr;
	}

	inline const Tree* find(const ID& id) const
	{
		if (mId == id)
		{
			return this;
		}

		for (const auto& child : mChildren)
		{
			if (const auto* result = child.find(id))
			{
				return result;
			}
		}

		return nullptr;
	}

	template<typename Func>
	inline const Tree* findByPayload(Func function) const
	{
		if (function(mPayload))
		{
			return this;
		}

		for (const auto& child : mChildren)
		{
			if (const auto* result = child.findByPayload(function))
			{
				return result;
			}
		}

		return nullptr;
	}

	inline u32 getSize() const
	{
		u32 size = 1;
		for (const auto& child : mChildren)
		{
			size += child.getSize();
		}
		return size;
	}

	inline void setPayload(Payload payload) { mPayload = std::forward<Payload>(payload); }
	inline const Payload& getPayload() const { return mPayload; }
	inline Payload& getPayload() { return mPayload; }
	
	inline void setId(ID id) { mId = std::forward<ID>(id); }
	inline const ID& getId() const { return mId; }
	inline ID& getId() { return mId; }
	
	inline bool hasParent() const { return mParent != nullptr; }
	inline Tree* getParent() { return mParent; }
	inline const Children& getChildren() const { return mChildren; }

private:
	ID mId;
	Payload mPayload;
	Tree* mParent;
	Children mChildren;
};

} // namespace erm
