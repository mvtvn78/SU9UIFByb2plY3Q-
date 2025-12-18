package com.maivantien.lab303_new.service.impl;
import lombok.AllArgsConstructor;
import org.springframework.stereotype.Service;
import com.maivantien.lab303_new.entity.Category;
import com.maivantien.lab303_new.service.CategoryService;
import com.maivantien.lab303_new.repository.CategoryRepository;
import java.util.List;
import java.util.Optional;
@Service
@AllArgsConstructor
public class CategoryServiceImpl implements CategoryService {
    private CategoryRepository categoryRepository;
    @Override
    public Category createCategory(Category category) {
        return categoryRepository.save(category);
    }
    @Override
    public Category getCategoryById(Long categoryId) {
        Optional<Category> optionalCategory = categoryRepository.findById(categoryId);
        return optionalCategory.get();
    }
    @Override
    public List<Category> getAllCategorys() {
        return categoryRepository.findAll();
    }
    @Override
    public Category updateCategory(Category category) {
        Category existingCategory = categoryRepository.findById(category.getCategoryId()).get();
        existingCategory.setName(category.getName());
        existingCategory.setDescription(category.getDescription());
        existingCategory.setDescription(category.getDescription());
        existingCategory.setParentCategory(category);
        Category updatedCategory = categoryRepository.save(existingCategory);
        return updatedCategory;
    }
    @Override
    public void deleteCategory(Long categoryId) {
        categoryRepository.deleteById(categoryId);
    }
}
